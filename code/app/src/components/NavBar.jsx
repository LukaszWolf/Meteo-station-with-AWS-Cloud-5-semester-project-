/**
 * @file NavBar.jsx
 * @description The main navigation header component.
 * It provides links to different sections of the page (Live Data, Forecast, History)
 * and a dynamic authentication button.
 */

/**
 * @component
 * @description Renders the top navigation bar.
 * Sticky positioned at the top of the viewport.
 *
 * @param {Object} props
 * @param {string} props.LogoutLoginText - The text to display on the action button (e.g., "Login" or "Logout").
 * @param {Function} props.onAuthClick - Callback function triggered when the action button is clicked.
 *
 * @returns {JSX.Element} The navigation header.
 */
export default function NavBar({ LogoutLoginText, onAuthClick }) {
  return (
    <header className="site-header">
      <div className="container">
        <a className="brand" href="/">Meteo<span>•</span>App</a>
        <nav aria-label="Główne">
          <ul className="menu">
            <li><a href="#live-data">Dane na żywo</a></li>
            <li><span>•</span></li>
            <li><a href="#forecast">Prognoza pogody</a></li>
            <li><span>•</span></li>
            <li><a href="#history">Historia</a></li>
            <li><span>•</span></li>
            <li><a href="#pairing">Parowanie</a></li>
            <li>
              <button className="cta" onClick={onAuthClick}>
                {LogoutLoginText}
              </button>
            </li>
          </ul>
        </nav>
      </div>
    </header>
  );
}