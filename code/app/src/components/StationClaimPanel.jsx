/**
 * @file StationClaimPanel.jsx
 * @description A presentational component (form) for claiming/pairing a new ESP32 device.
 */

import React from "react";
import "../StationClaimPanel.css";

/**
 * @component
 * @description Renders the inputs for Thing Name and Nonce code to pair a device via MQTT.
 *
 * @param {Object} props
 * @param {string} props.thing - The current value of the Thing Name input.
 * @param {Function} props.setThing - State setter for Thing Name.
 * @param {string} props.nonce - The current value of the Nonce (pairing code) input.
 * @param {Function} props.setNonce - State setter for Nonce.
 * @param {Function} props.handleClaim - Callback function triggered on button click.
 * @param {string} props.claimStatus - Status message to display (e.g., "Success", "Error").
 *
 * @returns {JSX.Element} The claiming form section.
 */
export default function StationClaimPanel({ 
  thing, 
  setThing, 
  nonce, 
  setNonce, 
  handleClaim, 
  claimStatus 
}) {
  return (
    <section className="station-claim-section">
      <div className="city-search-card">
        <div className="city-search-header">
          <div>
            <h2 className="city-search-title">Połącz nową stację</h2>
            <p className="city-search-subtitle">Wprowadź dane urządzenia</p>
          </div>
        </div>

        <div className="selected-city-wrapper" style={{ borderTop: "none", marginTop: 0 }}>
          <div className="station-form-grid">
            
            <div className="claim-input-group">
              <label className="claim-label">Nazwa urządzenia</label>
              <input
                className="city-search-input"
                placeholder="np. station-001"
                value={thing}
                onChange={(e) => setThing(e.target.value)}
              />
            </div>

            <div className="claim-input-group">
              <label className="claim-label">Kod autoryzacji</label>
              <input
                className="city-search-input"
                placeholder="Wpisz kod z ESP"
                value={nonce}
                onChange={(e) => setNonce(e.target.value)}
              />
            </div>

            <button 
              className="cta claim-button" 
              onClick={handleClaim}
              style={{ height: "42px", borderRadius: "12px" }}
            >
              Sparuj stację
            </button>
            
          </div>

          {claimStatus && (
            <div className={`claim-status-box ${claimStatus.includes("Błąd") ? "error" : claimStatus.includes("pomyślnie") ? "success" : ""}`}>
              {claimStatus}
            </div>
          )}
        </div>
      </div>
    </section>
  );
}