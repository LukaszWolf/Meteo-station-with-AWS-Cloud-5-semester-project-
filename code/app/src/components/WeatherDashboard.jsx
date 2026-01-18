/**
 * @file WeatherDashboard.jsx
 * @description A visual dashboard component that displays real-time weather data received from the MQTT broker.
 * It changes visual styles based on the UV index level.
 */

import "../WeatherDashboard.css";

/**
 * Determines the UV severity level string based on the numeric index.
 * Used to apply dynamic CSS classes for background gradients.
 *
 * @param {number|null} uv - The UV Index value (0-11+).
 * @returns {string} The severity level ('low', 'moderate', 'high', 'very-high', 'extreme').
 */
const getUvLevel = (uv) => {
  if (uv == null) return "low";
  if (uv <= 2) return "low";
  if (uv <= 5) return "moderate";
  if (uv <= 7) return "high";
  if (uv <= 10) return "very-high";
  return "extreme";
};

/**
 * @component
 * @description Displays tiles with Indoor and Outdoor weather metrics.
 *
 * @param {Object} props
 * @param {Object} props.data - The latest measurement object.
 * @param {number|null} props.data.indoorTemp - Indoor temperature in Celsius.
 * @param {number|null} props.data.outdoorTemp - Outdoor temperature in Celsius.
 * @param {number|null} props.data.pressure - Atmospheric pressure in hPa.
 * @param {number|null} props.data.humidity - Relative humidity in %.
 * @param {number|null} props.data.uvIndex - UV Index.
 *
 * @returns {JSX.Element} The dashboard grid layout.
 */
export default function WeatherDashboard({ data }) {
  const { indoorTemp, outdoorTemp, pressure, humidity, uvIndex } = data || {};
  const uvLevel = getUvLevel(uvIndex);

  return (
    <section className="dash-main-grid">
      {/* CARD: OUTDOOR (Dynamic background based on UV) */}
      <div className={`dash-card dash-outdoor uv-${uvLevel}`}>
        <div className="dash-section-header">
          <span className="dash-status-dot"></span>
          <div className="dash-section-title">NA ZEWNĄTRZ</div>
        </div>
        
        <div className="dash-main-content">
          <div className="dash-temp-display">
            {outdoorTemp != null ? `${outdoorTemp.toFixed(1)}°` : "—"}
            <span className="dash-unit">C</span>
          </div>
          
          <div className="dash-details-grid">
            <div className="dash-detail-item">
              <span className="dash-detail-label">Ciśnienie</span>
              <span className="dash-detail-value">{pressure != null ? `${pressure.toFixed(0)} hPa` : "—"}</span>
            </div>
            <div className="dash-detail-item">
              <span className="dash-detail-label">Wilgotność</span>
              <span className="dash-detail-value">{humidity != null ? `${humidity.toFixed(0)}%` : "—"}</span>
            </div>
            <div className="dash-detail-item">
              <span className="dash-detail-label">Indeks UV</span>
              <span className="dash-detail-value">{uvIndex != null ? uvIndex.toFixed(1) : "—"}</span>
            </div>
          </div>
        </div>
      </div>

      {/* CARD: INDOOR */}
      <div className="dash-card dash-indoor">
        <div className="dash-section-header">
          <span className="dash-status-dot"></span>
          <div className="dash-section-title">W DOMU</div>
        </div>
        
        <div className="dash-main-content">
          <div className="dash-temp-display">
            {indoorTemp != null ? `${indoorTemp.toFixed(1)}°` : "—"}
            <span className="dash-unit">C</span>
          </div>
        </div>
      </div>
    </section>
  );
}