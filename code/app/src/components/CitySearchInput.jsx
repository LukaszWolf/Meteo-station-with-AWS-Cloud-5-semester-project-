/**
 * @file CitySearchInput.jsx
 * @description A presentational component rendering the text input for city searching.
 */

/**
 * @component
 * @description Renders a styled input field. It is a controlled component driven by the parent's state.
 *
 * @param {Object} props
 * @param {string} props.query - The current value of the input field.
 * @param {Function} props.onChange - Callback function fired on every keystroke. Receives the new string value.
 *
 * @returns {JSX.Element} The input element.
 */
export default function CitySearchInput({ query, onChange }) {
    return (
        <input
            className="city-search-input"
            type="text"
            value={query}
            onChange={(e) => onChange(e.target.value)}
            placeholder="Wpisz nazwę miasta"
            aria-label="Wyszukaj miasto"
        />
    );
}