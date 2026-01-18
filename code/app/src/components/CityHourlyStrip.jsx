/**
 * @file CityHourlyStrip.jsx
 * @description A responsive carousel component for displaying hourly weather forecast tiles (24h).
 * Similar to CityDailyStrip, it handles horizontal scrolling and responsive resizing.
 */

import { useEffect, useState } from "react";
import HourlyTile from "./HourlyTile";

/**
 * @component
 * @description Renders a horizontal strip of hourly forecast tiles.
 *
 * @param {Object} props
 * @param {Array<Object>} props.items - An array of hourly forecast data objects.
 * Each item contains: { time, displayTime, temp, hum, code }.
 *
 * @returns {JSX.Element|null} The carousel component or null if items array is empty.
 */
export default function CityHourlyStrip({ items }) {
  if (!items || items.length === 0) return null;

  /**
   * Determines the number of visible tiles based on viewport width.
   * @returns {number} Tiles count (3-6).
   */
  const getTilesPerPage = () => {
    if (typeof window === "undefined") return 3;
    const w = window.innerWidth;

    if (w < 480) return 3;   // Very small screens
    if (w < 768) return 4;   // Mobile
    if (w < 1024) return 5;  // Tablet
    return 6;                // Desktop
  };

  const [tilesPerPage, setTilesPerPage] = useState(getTilesPerPage);
  const [startIndex, setStartIndex] = useState(0);

  /**
   * Effect: Handle window resize.
   */
  useEffect(() => {
    const onResize = () => {
      const next = getTilesPerPage();
      setTilesPerPage(next);
      setStartIndex(0); 
    };
    window.addEventListener("resize", onResize);
    return () => window.removeEventListener("resize", onResize);
  }, []);

  const maxStart = Math.max(0, items.length - tilesPerPage);
  const canGoPrev = startIndex > 0;
  const canGoNext = startIndex < maxStart;

  const safeStart = Math.min(startIndex, maxStart);
  const visibleItems = items.slice(safeStart, safeStart + tilesPerPage);

  const handlePrev = () => {
    if (!canGoPrev) return;
    setStartIndex((prev) => Math.max(0, prev - tilesPerPage));
  };

  const handleNext = () => {
    if (!canGoNext) return;
    setStartIndex((prev) => {
      const candidate = prev + tilesPerPage;
      return candidate >= maxStart ? maxStart : candidate;
    });
  };

  return (
    <div className="weather-strip">
      <button
        onClick={handlePrev}
        disabled={!canGoPrev}
        className="weather-strip-btn"
        aria-label="Previous hours"
      >
        ◀
      </button>

      <div className="weather-tiles-row">
        {visibleItems.map((item) => (
          <HourlyTile key={item.time} item={item} />
        ))}
      </div>

      <button
        onClick={handleNext}
        disabled={!canGoNext}
        className="weather-strip-btn"
        aria-label="Next hours"
      >
        ▶
      </button>
    </div>
  );
}