/**
 * @file CitySearch.jsx
 * @description A container component for searching locations via Open-Meteo Geocoding API.
 * It handles the search state, debouncing, and fetching logic.
 */

import { useEffect, useState, useRef } from "react";
import CitySearchInput from "./CitySearchInput";
import CitySuggestionsList from "./CitySuggestionsList";
import SelectedCityInfo from "./SelectedCityInfo";

/** * Open-Meteo Geocoding API endpoint 
 * @constant {string}
 */
const GEO_URL = "https://geocoding-api.open-meteo.com/v1/search";

/**
 * @component
 * @description Orchestrates the city search workflow.
 * Features:
 * - Input field for typing city name.
 * - Debounced API calls to prevent flooding the server.
 * - Displays a list of suggestions.
 * - Shows weather details for the selected city.
 *
 * @returns {JSX.Element} The complete weather forecast section.
 */
export default function CitySearch() {
  const [query, setQuery] = useState("");
  const [suggestions, setSuggestions] = useState([]);
  const [selectedCity, setSelectedCity] = useState(null);
  
  /** * Ref to store the timeout ID for the debounce logic. 
   * Allows clearing the timeout across re-renders without triggering new ones.
   */
  const debounceRef = useRef(null);

  /**
   * Effect handles the debounced search logic.
   * Triggers API call only after the user stops typing for 500ms.
   */
  useEffect(() => {
    // Clear previous timer on every keystroke to reset the countdown
    if (debounceRef.current) {
      clearTimeout(debounceRef.current);
    }

    const trimmed = query.trim();

    if (trimmed.length < 2) {
      setSuggestions([]);
      return;
    }

    // Set new timer
    debounceRef.current = setTimeout(() => {
      const url = `${GEO_URL}?name=${encodeURIComponent(trimmed)}&count=5&language=pl&format=json`;

      fetch(url)
        .then((res) => {
          if (!res.ok) throw new Error("Error fetching city suggestions");
          return res.json();
        })
        .then((data) => {
          setSuggestions(data.results || []);
        })
        .catch((err) => {
          console.error(err);
          setSuggestions([]);
        });
    }, 500); // 500ms delay

    // Cleanup function: clears timer if component unmounts
    return () => {
      if (debounceRef.current) clearTimeout(debounceRef.current);
    };
  }, [query]);

  return (
    <section className="city-search-section">
      <div className="city-search-card">
        <div className="city-search-header">
          <div>
            <h2 className="city-search-title">Prognoza pogody</h2>
            <p className="city-search-subtitle">
              Wyszukaj miejscowość i sprawdź prognozę godzinową oraz dzienną.
            </p>
          </div>
        </div>

        <div className="city-search-input-row">
          <CitySearchInput
            query={query}
            onChange={(newQuery) => setQuery(newQuery)}
          />
          <CitySuggestionsList
            suggestions={suggestions}
            onSelect={(city) => {
              console.log("Selected city:", city);
              setSelectedCity(city);
              setSuggestions([]); // Clear suggestions after selection
            }}
          />
        </div>

        <div className="selected-city-wrapper">
          <SelectedCityInfo city={selectedCity} />
        </div>
      </div>
    </section>
  );
}