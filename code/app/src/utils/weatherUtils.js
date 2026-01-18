/**
 * @file weatherUtils.js
 * @description Utility functions for weather data processing.
 * Contains mappings from WMO weather codes to visual icons (emojis).
 */

/**
 * Converts a WMO numeric weather code into a corresponding emoji icon.
 * This helper is used by forecast tiles to visualize weather conditions from
 * Open-Meteo API data.
 *
 * @param {number|null} code - The WMO weather code (e.g., 0, 1, 45, 95).
 * @returns {string} An emoji string representing the weather.
 * Returns '' if the code is null or unrecognized.
 */

export const getWeatherIcon = (code) => {
  if (code == null) return '❓';

  // WMO Codes (Open-Meteo)
  switch (code) {
    case 0:
      return '☀️';  // Clear sky
    case 1:
      return '🌤️';  // Mainly clear
    case 2:
      return '⛅';  // Partly cloudy
    case 3:
      return '☁️';  // Overcast
    case 45:
    case 48:
      return '🌫️';  // Fog
    case 51:
    case 53:
    case 55:
      return '🌦️';  // Drizzle
    case 56:
    case 57:
      return '🌨️';  // Freezing Drizzle
    case 61:
    case 63:
    case 65:
      return '🌧️';  // Rain
    case 66:
    case 67:
      return '🥶';  // Freezing Rain
    case 71:
    case 73:
    case 75:
      return '❄️';  // Snow
    case 77:
      return '🌨️';  // Snow Grains
    case 80:
    case 81:
    case 82:
      return '🌧️';  // Rain showers
    case 85:
    case 86:
      return '❄️';  // Snow showers
    case 95:
      return '⛈️';  // Thunderstorm
    case 96:
    case 99:
      return '⛈️';  // Thunderstorm with hail
    default:
      return '❓';
  }
};