/**
 * @file useStationData.js
 * @description Custom hook for fetching and managing station telemetry data
 * from AWS S3. It retrieves historical data files, parses them, and formats
 * them for the dashboard.
 */

import {Auth, Storage} from 'aws-amplify';
import {useEffect, useState} from 'react';

/**
 * Helper: Maps raw JSON data from S3 to the dashboard's expected format.
 * Performs unit conversions (e.g., dividing raw temp by 10).
 * @param {Object} json - Raw JSON object from the station.
 * @returns {Object} Normalized data object.
 */
const mapJsonToDashboardData = (json) => {
  return {
    outdoorTemp: json.outdoorTemperatureRead != null ?
        json.outdoorTemperatureRead / 10 :
        null,
    humidity: json.humidityRead ?? null,
    pressure: json.pressureRead ?? null,
    uvIndex: json.uvIndexRead != null ? json.uvIndexRead / 10 : null,
    indoorTemp:
        json.indoorTemperatureRead != null ? json.indoorTemperatureRead : null,
    lastUpdate: json.ts ?? null,
  };
};

/**
 * @function useStationData
 * @description Manages data fetching for the station dashboard.
 *
 * @param {Object} user - The authenticated user object.
 * @returns {Object} Data object containing:
 * - history {Array}: Chronologically sorted array of historical measurements.
 * - measurement {Object|null}: The single most recent measurement.
 * - loading {boolean}: Fetching status.
 * - reloadData {Function}: Function to manually trigger a data refresh.
 */
export function useStationData(user) {
  const [history, setHistory] = useState([]);
  const [measurement, setMeasurement] = useState(null);
  const [loading, setLoading] = useState(false);

  const loadFiles = async () => {
    if (!user) return;

    setLoading(true);
    try {
      const creds = await Auth.currentCredentials();
      const id = creds.identityId;

      // 1. List all files in the user's station folder
      const {results} =
          await Storage.list(`users/${id}/stations/`, {level: 'public'});

      // 2. Sort files by modification date (newest first)
      results.sort(
          (a, b) => new Date(b.lastModified) - new Date(a.lastModified));

      // 3. Limit to the 48 most recent files for performance
      const recentFiles = results.slice(0, 48);

      if (recentFiles.length > 0) {
        const historyData = [];

        // Parallel download of file contents
        await Promise.all(recentFiles.map(async (file) => {
          try {
            const result =
                await Storage.get(file.key, {download: true, level: 'public'});
            const text = await result.Body.text();
            const obj = JSON.parse(text);
            historyData.push(mapJsonToDashboardData(obj));
          } catch (err) {
            console.warn(`Error parsing file ${file.key}`, err);
          }
        }));

        // Sort history chronologically (oldest -> newest) for charts
        historyData.sort((a, b) => (a.lastUpdate || 0) - (b.lastUpdate || 0));
        setHistory(historyData);

        // Set the latest measurement
        if (historyData.length > 0) {
          setMeasurement(historyData[historyData.length - 1]);
        }
      }
    } catch (err) {
      console.error('Data fetch error:', err);
    } finally {
      setLoading(false);
    }
  };

  // Auto-load when user logs in
  useEffect(() => {
    if (user) {
      loadFiles();
    } else {
      setHistory([]);
      setMeasurement(null);
    }
  }, [user]);

  return {history, measurement, loading, reloadData: loadFiles};
}