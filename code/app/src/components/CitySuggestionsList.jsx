/**
 * @file CitySuggestionsList.jsx
 * @description A UI component that renders a list of city search results.
 * It handles the display of city names, country codes, and administrative regions.
 */

/**
 * @component
 * @description Renders an unordered list of city suggestions.
 * Triggers a selection event when a user clicks on a list item.
 *
 * @param {Object} props
 * @param {Array<Object>} props.suggestions - List of city objects returned by the Geocoding API.
 * @param {number} [props.suggestions[].id] - Unique identifier for the city.
 * @param {string} props.suggestions[].name - Name of the city.
 * @param {string} props.suggestions[].country - Country name.
 * @param {string} [props.suggestions[].admin1] - Administrative region (e.g., Voivodeship/State).
 * @param {number} props.suggestions[].latitude - Geographical latitude.
 * @param {number} props.suggestions[].longitude - Geographical longitude.
 * @param {Function} props.onSelect - Callback function triggered when a city is clicked. Receives the city object.
 *
 * @returns {JSX.Element|null} The list of suggestions or null if the array is empty.
 */
export default function CitySuggestionsList({ suggestions, onSelect }) {
  if (!suggestions || suggestions.length === 0) {
    return null;
  }

  return (
    <ul className="city-suggestions">
      {suggestions.map((city) => {
        // Fallback key generation if ID is missing
        const key =
          city.id ?? `${city.name}-${city.latitude}-${city.longitude}`;

        return (
          <li
            key={key}
            className="city-suggestion"
            onClick={() => onSelect(city)}
          >
            {city.name}, {city.country}
            {city.admin1 ? ` (${city.admin1})` : ""}
          </li>
        );
      })}
    </ul>
  );
}