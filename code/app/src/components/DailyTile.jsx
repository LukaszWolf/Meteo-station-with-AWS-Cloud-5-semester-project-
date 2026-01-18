/**
 * @file DailyTile.jsx
 * @description A presentational component representing a single day in the forecast strip.
 * Displays the day name, weather icon, min/max temperatures, and sun times.
 */

import { getWeatherIcon } from "../utils/weatherUtils";

/**
 * @component
 * @description Renders a single tile for the daily forecast carousel.
 *
 * @param {Object} props
 * @param {Object} props.item - The daily weather data object.
 * @param {string} props.item.label - Formatted date label (e.g., "Mon 12.05").
 * @param {number} props.item.code - WMO Weather code used to fetch the icon.
 * @param {number} props.item.tMax - Maximum temperature for the day.
 * @param {number} props.item.tMin - Minimum temperature for the day.
 * @param {string} [props.item.sunrise] - Sunrise time (HH:MM).
 * @param {string} [props.item.sunset] - Sunset time (HH:MM).
 *
 * @returns {JSX.Element} A styled tile with weather details.
 */
export default function DailyTile({ item }) {
  return (
    <div
      className="daily-tile"
      style={{
        padding: "10px 12px",
        borderRadius: 12,
        background: "rgba(255,255,255,0.12)",
        backdropFilter: "blur(4px)",
        border: "1px solid rgba(255,255,255,0.2)",
        display: "flex",
        flexDirection: "column",
        alignItems: "center",
        fontSize: 12,
        minWidth: "80px"
      }}
    >
      <div style={{ fontWeight: 600, marginBottom: 4 }}>
        {item.label}
      </div>

      {/* Weather Icon based on WMO code */}
      <div style={{ fontSize: "28px", marginBottom: "4px" }}>
        {getWeatherIcon(item.code)}
      </div>

      <div style={{ fontSize: 16, fontWeight: 700, marginBottom: 2 }}>
        {Math.round(item.tMax)}° <span style={{ opacity: 0.6, fontSize: "0.9em" }}>/ {Math.round(item.tMin)}°</span>
      </div>

      <div style={{ fontSize: "10px", opacity: 0.8, marginTop: "4px", display: "flex", gap: "6px" }}>
        {item.sunrise && <span>↑ {item.sunrise}</span>}
        {item.sunset && <span>↓ {item.sunset}</span>}
      </div>
    </div>
  );
}