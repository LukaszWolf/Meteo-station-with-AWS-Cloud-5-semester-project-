/**
 * @file CityWeatherPanel.jsx
 * @description A container component responsible for processing raw weather data.
 * It transforms the Open-Meteo API response (separate arrays for time, temp, etc.) 
 * into structured objects suitable for the Hourly and Daily carousel components.
 */

import CityHourlyStrip from "./CityHourlyStrip";
import CityDailyStrip from "./CityDailyStrip";

/**
 * Helper: Extracts HH:MM time string from an ISO 8601 date string.
 * @param {string} isoString - Full date string (e.g., "2023-10-27T14:00").
 * @returns {string} Formatted time (e.g., "14:00").
 */
function formatTimeHHMM(isoString) {
  if (!isoString) return "";
  const idxT = isoString.indexOf("T");
  if (idxT === -1) return isoString;
  const timePart = isoString.slice(idxT + 1);
  return timePart.slice(0, 5);
}

/**
 * Helper: Formats a date string into a localized short label (e.g., "Wed 27.11").
 * Uses Polish locale (pl-PL).
 * @param {string} dateStr - Date string (YYYY-MM-DD).
 * @returns {string} Localized date label.
 */
function formatDateLabel(dateStr) {
  try {
    const d = new Date(dateStr);
    return d.toLocaleDateString("pl-PL", {
      weekday: "short",
      day: "2-digit",
      month: "2-digit",
    });
  } catch {
    return dateStr;
  }
}

/**
 * @component
 * @description Orchestrates the display of weather forecasts.
 * It parses the `weather` prop to extract:
 * 1. Hourly forecast: Next 24 hours from the current time.
 * 2. Daily forecast: Up to 10 days of future data.
 *
 * @param {Object} props
 * @param {Object} props.weather - The raw response object from Open-Meteo API.
 * @param {Object} props.weather.hourly - Object containing arrays of hourly data (time, temp, codes).
 * @param {Object} props.weather.daily - Object containing arrays of daily data (time, min/max temp, sunrise/sunset).
 * @param {Object} [props.weather.current_weather] - Current weather data used for finding the start time index.
 *
 * @returns {JSX.Element|null} The forecast sections or null if data is missing.
 */
export default function CityWeatherPanel({ weather }) {
  if (!weather) return null;

  // ------ LOGIC: HOURLY FORECAST (Next 24h) ------
  let hourlyItems = [];
  if (
    weather.hourly &&
    weather.hourly.time &&
    weather.hourly.temperature_2m &&
    weather.hourly.weathercode
  ) {
    const times = weather.hourly.time;
    const len = times.length;
    const currentTime = weather.current_weather?.time;

    // Find the index of the next hour relative to current time
    let startIdx = 0;
    if (currentTime) {
      const idx = times.findIndex((t) => t > currentTime);
      if (idx !== -1) {
        startIdx = idx;
      }
    }

    // Slice exactly 24 hours from the start index
    const endIdx = Math.min(startIdx + 24, len);

    for (let i = startIdx; i < endIdx; i++) {
      const time = times[i];
      hourlyItems.push({
        time,
        displayTime: formatTimeHHMM(time),
        temp: weather.hourly.temperature_2m[i],
        hum: weather.hourly.relative_humidity_2m
          ? weather.hourly.relative_humidity_2m[i]
          : null,
        code: weather.hourly.weathercode[i],
      });
    }
  }

  // ------ LOGIC: DAILY FORECAST (Next 10 days) ------
  let dailyItems = [];
  if (
    weather.daily &&
    weather.daily.time &&
    weather.daily.temperature_2m_max &&
    weather.daily.temperature_2m_min
  ) {
    const count = Math.min(10, weather.daily.time.length);
    for (let i = 0; i < count; i++) {
      const date = weather.daily.time[i];
      dailyItems.push({
        date,
        label: formatDateLabel(date),
        tMax: weather.daily.temperature_2m_max[i],
        tMin: weather.daily.temperature_2m_min[i],
        sunrise: weather.daily.sunrise
          ? formatTimeHHMM(weather.daily.sunrise[i])
          : null,
        sunset: weather.daily.sunset
          ? formatTimeHHMM(weather.daily.sunset[i])
          : null,
        code: weather.daily.weathercode
          ? weather.daily.weathercode[i]
          : null,
      });
    }
  }

  return (
    <div style={{ marginTop: 16 }}>
      {/* SECTION: HOURLY */}
      {hourlyItems.length > 0 && (
        <div style={{ marginBottom: 24 }}>
          <h4>Prognoza godzinowa (24h)</h4>
          <CityHourlyStrip items={hourlyItems} />
        </div>
      )}

      {/* SECTION: DAILY */}
      {dailyItems.length > 0 && (
        <div>
          <h4>Prognoza dzienna (do 10 dni)</h4>
          <CityDailyStrip items={dailyItems} />
        </div>
      )}
    </div>
  );
}