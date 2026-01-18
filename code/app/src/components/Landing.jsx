/**
 * @file Landing.jsx
 * @description The landing page component displayed to unauthenticated users.
 * It explains the project's architecture (ESP32 + AWS), features, and user instructions.
 */

/**
 * @component
 * @description Renders the static landing page content.
 * Includes a hero section, feature list, "How it works" steps, and a CTA button for login.
 *
 * @param {Object} props
 * @param {Function} props.onSignIn - Callback function to trigger the AWS Cognito login/registration flow.
 *
 * @returns {JSX.Element} The landing page layout.
 */
export default function Landing({ onSignIn }) {
  return (
    <main className="landing">
      {/* HERO SECTION */}
      <section className="hero">
        <h1>Domowa stacja pogodowa z wykorzystaniem chmury</h1>
        <p className="lead">
          Zbieraj dane pogodowe przy pomocy bezprzewodowego modułu wyposażonego w czujniki. 
          Podgląd na żywo historia i prognoza pogody w aplikacji.
        </p>
        <div className="cta-row">
          <button className="btn btn-primary" onClick={onSignIn}>Zaloguj / Załóż konto</button>
        </div>
      </section>

      {/* PRODUCT OVERVIEW */}
      <section className="section">
        <h2>Co to jest?</h2>
        <p>
          To zestaw, w którego skład wchodzi stacja domowa z wyświetlaczem i moduł zewnętrzny.
          Moduł zewnętrzny wysyła dane do stacji w domu, stacja wysyła dane przez MQTT do chmury AWS, a aplikacja webowa prezentuje je w czytelnych kafelkach i na wykresach.
        </p>
              <ul className="feature-list">
          <li><b>Stacja</b> – Wyswietlanie danych, zegar i wiele innych.</li>     
          <li><b>Moduł zewnętrzny</b> – zbieranie danych pogodowych z czujników i przekazanie ich do stacji.</li>
          <li><b>Dane na żywo</b> – Pobieranie danych z chmury.</li>
          <li><b>Historia</b> – Historia pomiarów na wykresach.</li>
          <li><b>Bezpieczeństwo</b> – dane w S3, CORS, certyfikaty AWS.</li>
          <li><b>Konta użytkowników</b> – logowanie i rejestracja z wykorzystaniem Cognito.</li>

        </ul>
      </section>



{/* <section className="section">
        <h2>Podgląd aplikacji</h2>
        <div className="screenshot-placeholder" aria-label="Zrzut ekranu aplikacji (wkrótce)">
          
        </div>
      </section>
      */}
    </main>
  );
}