/**
 * @file App.jsx
 * @description Main entry point for the Cloud Meteo Station web application.
 * This file is responsible for:
 * 1. Configuring AWS Amplify and AWS IoT PubSub providers.
 * 2. Managing global authentication state via the `useAuth` hook.
 * 3. Orchestrating data fetching and station pairing via custom hooks.
 * 4. Rendering the main layout based on the user's authentication status (Guest vs. Logged In).
 *
 * @author Your Name
 */

import { Amplify } from "aws-amplify";
import { AWSIoTProvider } from "@aws-amplify/pubsub";
import awsconfig from "./aws-exports";

// UI Components
import NavBar from "./components/NavBar";
import Landing from "./components/Landing";
import Footer from "./components/Footer";
import WeatherDashboard from "./components/WeatherDashboard";
import StationClaimPanel from "./components/StationClaimPanel";
import CitySearch from "./components/CitySearch";
import HistoryPanel from "./components/HistoryPanel";

// Global Styles
import "./App.css";
import "./NavBar.css";
import "./Landing.css";
import "./Footer.css";
import "./WeatherDashboard.css";
import "./CitySearch.css";

// Custom Hooks (Business Logic)
import { useAuth } from "./hooks/useAuth";
import { useStationData } from "./hooks/useStationData";
import { useStationClaim } from "./hooks/useStationClaim";

// --- AWS CONFIGURATION ---

/**
 * Configure AWS Amplify with the auto-generated aws-exports file.
 * This setups Cognito User Pools and Identity Pools.
 */
Amplify.configure(awsconfig);

/**
 * Configure the AWS IoT PubSub provider over WebSockets (WSS).
 * This allows the app to subscribe to MQTT topics for real-time data.
 *
 * @constant {string} AWS_IOT_ENDPOINT - The specific AWS IoT endpoint for your region.
 */
Amplify.addPluggable(new AWSIoTProvider({
  aws_pubsub_region: "eu-north-1",
  aws_pubsub_endpoint: "wss://an7hi8lzvqru3-ats.iot.eu-north-1.amazonaws.com/mqtt",
}));

/**
 * @component
 * @description The root component of the application.
 * It serves as a container for the UI and delegates logic to custom hooks.
 *
 * @returns {JSX.Element} The rendered application layout.
 */
export default function App() {
  /**
   * 1. Authentication Hook
   * Handles user login/logout sessions and hub events.
   */
  const { user, signIn, signOut } = useAuth();

  /**
   * 2. Station Data Hook
   * Fetches historical data from S3 and current measurements.
   * Only active if a user is logged in.
   */
  const { history, measurement, reloadData } = useStationData(user);

  /**
   * 3. Station Claiming Hook
   * Manages the logic for pairing a new ESP32 device with the user's account via MQTT/API.
   * Calls `reloadData` upon successful claiming to refresh the dashboard.
   */
  const claim = useStationClaim(user, reloadData);

  // --- CONDITIONAL RENDERING ---

  // Render Landing Page for guests (unauthenticated users)
  if (!user) {
    return (
      <div>
        <NavBar LogoutLoginText="Zaloguj się" onAuthClick={signIn} />
        <Landing onSignIn={signIn} />
        <Footer />
      </div>
    );
  }

  // Render Main Dashboard for authenticated users
  return (
    <div>
      <NavBar LogoutLoginText="Wyloguj się" onAuthClick={signOut} />
      
      {/* Section: Live Data Dashboard */}
      <section id="live-data" className="page-section">
        {measurement && <WeatherDashboard data={measurement} />}
      </section>
    
      {/* Section: Weather Forecast (Open-Meteo) */}
      <section id="forecast" className="page-section">
        <CitySearch />
      </section>

      {/* Section: Historical Data Charts */}
      <section id="history" className="page-section">
        <HistoryPanel history={history} />
      </section>

      {/* Section: Device Pairing Form */}
      <section id="pairing" className="page-section">
        <StationClaimPanel 
          thing={claim.thing}
          setThing={claim.setThing}
          nonce={claim.nonce}
          setNonce={claim.setNonce}
          handleClaim={claim.handleClaim}
          claimStatus={claim.claimStatus}
        />
      </section>
      
      <Footer/>
    </div>
  );
}