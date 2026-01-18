/**
 * @file HourlyTile.jsx
 * @description A simple presentational component representing a single hour in the forecast carousel.
 * It displays the time, weather icon, temperature, and optional humidity.
 */

import { getWeatherIcon } from "../utils/weatherUtils";

/**
 * @component
 * @description Renders a single tile for the hourly forecast.
 *
 * @param {Object} props
 * @param {Object} props.item - The hourly forecast data object.
 * @param {string} props.item.displayTime - Formatted time string (e.g., "14:00").
 * @param {number} props.item.code - WMO weather code used to fetch the correct icon.
 * @param {number} props.item.temp - Temperature in Celsius.
 * @param {number} [props.item.hum] - Relative humidity percentage (optional).
 *
 * @returns {JSX.Element} A styled tile component.
 */
export default function HourlyTile({ item }) {
  return (
    <div
      className="hourly-tile"
      style={{
        padding: "8px 10px",
        borderRadius: 12,
        background: "rgba(255,255,255,0.12)",
        backdropFilter: "blur(4px)",
        border: "1px solid rgba(255,255,255,0.2)",
        display: "flex",
        flexDirection: "column",
        alignItems: "center",
        fontSize: 12,
        minWidth: "70px"
      }}
    >
      <div style={{ fontWeight: 600, marginBottom: 4 }}>
        {item.displayTime}
      </div>
      
      {/* Weather Icon */}
      <div style={{ fontSize: "24px", marginBottom: "4px" }}>
        {getWeatherIcon(item.code)}
      </div>

      <div style={{ fontSize: 18, fontWeight: 700 }}>
        {Math.round(item.temp)}°C
      </div>
      
      {item.hum != null && (
        <div style={{ opacity: 0.8, fontSize: "10px", marginTop: "2px" }}>
          {item.hum}%
        </div>
      )}
    </div>
  );
}