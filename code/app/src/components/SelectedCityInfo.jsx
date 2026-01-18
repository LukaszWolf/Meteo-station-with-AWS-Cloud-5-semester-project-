/**
 * @file SelectedCityInfo.jsx
 * @description A container component that fetches and displays detailed weather information for a specific city.
 * It communicates with the Open-Meteo API to get current weather, hourly, and daily forecasts.
 */

import { useEffect, useState } from "react";
import CityWeatherPanel from "./CityWeatherPanel";

/** * Base URL for the Open-Meteo Forecast API */
const FORECAST_URL = "https://api.open-meteo.com/v1/forecast";

/**
 * Helper: Extracts "HH:MM" time string from an ISO 8601 date string.
 * @param {string} isoString - Full ISO date string.
 * @returns {string} Formatted time string.
 */
function formatTimeHHMM(isoString) {
  if (!isoString) return "";
  const idxT = isoString.indexOf("T");
  if (idxT === -1) return isoString;
  const timePart = isoString.slice(idxT + 1);
  return timePart.slice(0, 5); // "HH:MM"
}

/**
 * @component
 * @description Renders current weather details (Temp, Wind, Sunrise/Sunset) and the forecast panel.
 * Triggers a data fetch whenever the `city` prop changes.
 *
 * @param {Object} props
 * @param {Object|null} props.city - The selected city object containing geographical coordinates.
 * @param {string} props.city.name - City name.
 * @param {string} props.city.country - Country name.
 * @param {number} props.city.latitude - Latitude for API request.
 * @param {number} props.city.longitude - Longitude for API request.
 *
 * @returns {JSX.Element} The weather details view.
 */
export default function SelectedCityInfo({ city }) {
  const [weather, setWeather] = useState(null);
  const [weatherLoading, setWeatherLoading] = useState(false);
  const [weatherError, setWeatherError] = useState("");

  /**
   * Effect: Fetches weather data from Open-Meteo API when `city` updates.
   */
  useEffect(() => {
    if (!city) {
      setWeather(null);
      setWeatherError("");
      return;
    }

    const params = new URLSearchParams({
      latitude: String(city.latitude),
      longitude: String(city.longitude),
      current_weather: "true",
      hourly: "temperature_2m,relative_humidity_2m,weathercode",
      daily:
        "temperature_2m_max,temperature_2m_min,sunrise,sunset,weathercode",
      timezone: "auto",
      forecast_days: "10",
    });

    const url = `${FORECAST_URL}?${params.toString()}`;

    setWeatherLoading(true);
    setWeatherError("");

    fetch(url)
      .then((res) => {
        if (!res.ok) {
          throw new Error("Błąd pobierania pogody");
        }
        return res.json();
      })
      .then((data) => {
        setWeather(data);
      })
      .catch((err) => {
        console.error(err);
        setWeather(null);
        setWeatherError(String(err));
      })
      .finally(() => {
        setWeatherLoading(false);
      });
  }, [city]);

  if (!city) {
    return <p>Nie wybrano miasta.</p>;
  }

  // --- Calculate sunrise/sunset for the current day ---
  let todaySunrise = null;
  let todaySunset = null;

  if (
    weather &&
    weather.daily &&
    weather.daily.time &&
    weather.daily.sunrise &&
    weather.daily.sunset &&
    weather.current_weather?.time
  ) {
    // Find the index corresponding to today's date in the daily forecast array
    const currentDate = weather.current_weather.time.slice(0, 10); // "YYYY-MM-DD"
    const idx = weather.daily.time.findIndex((d) => d === currentDate);
    if (idx !== -1) {
      todaySunrise = weather.daily.sunrise[idx] || null;
      todaySunset = weather.daily.sunset[idx] || null;
    }
  }

  return (
    <div style={{ marginTop: 16 }}>
      <h3 className="selected-city-title">
        Wybrane miasto:{" "}
        <span>
          {city.name}, {city.country}
          {city.admin1 ? ` (${city.admin1})` : ""}
        </span>
      </h3>

      {weatherLoading && <p>Ładuję pogodę...</p>}
      {weatherError && (
        <p style={{ color: "red" }}>Błąd pobierania pogody: {weatherError}</p>
      )}

      {weather && weather.current_weather && (
        <div>
          <p>Aktualna temperatura: {weather.current_weather.temperature} °C</p>
          <p>Wiatr: {weather.current_weather.windspeed} km/h</p>

          {todaySunrise && todaySunset && (
            <>
              <p>Wschód słońca: {formatTimeHHMM(todaySunrise)}</p>
              <p>Zachód słońca: {formatTimeHHMM(todaySunset)}</p>
            </>
          )}
        </div>
      )}

      {/* Render detailed forecast panel */}
      <CityWeatherPanel weather={weather} />
    </div>
  );
}