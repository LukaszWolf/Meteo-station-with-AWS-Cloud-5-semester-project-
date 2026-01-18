/**
 * @file CityDailyStrip.jsx
 * @description A responsive carousel component that displays daily weather forecast tiles.
 * It automatically adjusts the number of visible tiles based on the screen width and handles pagination.
 */

import { useEffect, useState } from "react";
import DailyTile from "./DailyTile";

/**
 * @component
 * @description Renders a horizontal strip of daily forecast tiles with navigation buttons.
 *
 * @param {Object} props
 * @param {Array<Object>} props.items - An array of daily forecast data objects.
 * Each item contains: { date, label, tMax, tMin, sunrise, sunset, code }.
 *
 * @returns {JSX.Element|null} The carousel component or null if items array is empty.
 */
export default function CityDailyStrip({ items }) {
  if (!items || items.length === 0) return null;

  /**
   * Calculates how many tiles can fit on the current screen width.
   * @returns {number} Number of tiles to display (3 to 6).
   */
  const getTilesPerPage = () => {
    if (typeof window === "undefined") return 3;
    const w = window.innerWidth;

    if (w < 480) return 3;    // Mobile (portrait)
    if (w < 768) return 4;    // Mobile (landscape)
    if (w < 1024) return 5;   // Tablet / Small Laptop
    return 6;                 // Desktop
  };

  const [tilesPerPage, setTilesPerPage] = useState(getTilesPerPage);
  const [startIndex, setStartIndex] = useState(0);

  /**
   * Effect: Listens for window resize events to update the layout dynamically.
   * Resets the view to the start (index 0) on resize to prevent layout glitches.
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

  // Calculate limits to prevent scrolling past the end
  const maxStart = Math.max(0, items.length - tilesPerPage);

  const canGoPrev = startIndex > 0;
  const canGoNext = startIndex < maxStart;

  // Safe slice logic for rendering
  const safeStart = Math.min(startIndex, maxStart);
  const visibleItems = items.slice(safeStart, safeStart + tilesPerPage);

  /**
   * Slides the carousel to the left (previous page).
   */
  const handlePrev = () => {
    if (!canGoPrev) return;
    setStartIndex((prev) => Math.max(0, prev - tilesPerPage));
  };

  /**
   * Slides the carousel to the right (next page).
   */
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
        aria-label="Previous days"
      >
        ◀
      </button>

      <div className="weather-tiles-row">
        {visibleItems.map((item) => (
          <DailyTile key={item.date} item={item} />
        ))}
      </div>

      <button
        onClick={handleNext}
        disabled={!canGoNext}
        className="weather-strip-btn"
        aria-label="Next days"
      >
        ▶
      </button>
    </div>
  );
} 