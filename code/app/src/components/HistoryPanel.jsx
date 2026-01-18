/**
 * @file HistoryPanel.jsx
 * @description A container component that organizes historical data charts.
 * It renders two instances of `HistoryChart` (Outdoor and Indoor) if data is available.
 */

import HistoryChart from "./HistoryChart";

/**
 * @component
 * @description Renders the history section with charts for outdoor and indoor temperatures.
 *
 * @param {Object} props
 * @param {Array<Object>} props.history - An array of historical measurement objects.
 * Each object should contain timestamps and temperature readings.
 *
 * @returns {JSX.Element} The history panel with charts or a "No Data" message.
 */
export default function HistoryPanel({ history }) {
  console.log("HistoryPanel history:", history);
  
  if (!history || history.length === 0) {
    return (
      <section className="city-search-section">
        <div className="city-search-card" style={{ textAlign: "center", padding: "20px" }}>
          <p style={{ opacity: 0.7 }}>Brak danych historycznych.</p>
        </div>
      </section>
    );
  }

  return (
    <section className="city-search-section">
      <div className="city-search-card">
        
        <h2 className="city-search-title" style={{ marginBottom: "4px" }}>
          Historia pomiarów
        </h2>
        <p className="city-search-subtitle" style={{ marginBottom: "20px" }}>
          Wykresy temperatury z ostatnich godzin
        </p>

        {/* Outdoor Chart Configuration */}
        <HistoryChart 
          history={history} 
          dataKey="outdoorTemp" 
          color="#4f8cff" 
          title="Na zewnątrz" 
          gradientId="gradOutdoor"
        />

        {/* Indoor Chart Configuration */}
        <HistoryChart 
          history={history} 
          dataKey="indoorTemp" 
          color="#34d399" 
          title="W domu" 
          gradientId="gradIndoor"
        />

      </div>
    </section>
  );
}