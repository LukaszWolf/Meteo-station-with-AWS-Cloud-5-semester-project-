/**
 * @file HistoryChart.jsx
 * @description A reusable SVG line chart component designed to visualize temperature trends.
 * It features a gradient area fill, data points with values, and automatic horizontal scrolling 
 * to the most recent data point.
 */

import { useMemo, useRef, useEffect } from "react";

/**
 * Helper: Extracts the timestamp from a data point.
 * Handles different property names ('ts' or 'lastUpdate') for compatibility.
 * @param {Object} m - Data point object.
 * @returns {number|null} Timestamp in milliseconds or null.
 */
function getTs(m) { return m.ts ?? m.lastUpdate ?? null; }

/**
 * Helper: Sorts the history array chronologically (oldest to newest).
 * @param {Array<Object>} history - Array of unsorted data points.
 * @returns {Array<Object>} Sorted array.
 */
function sortByTs(history) {
  return [...history].sort((a, b) => (getTs(a) ?? 0) - (getTs(b) ?? 0));
}

/**
 * Helper: Formats timestamp to "HH:MM" string.
 * @param {number} ts - Timestamp.
 * @returns {string} Formatted time string.
 */
function formatTimeHHMM(ts) {
  if (ts == null) return "";
  const d = new Date(ts);
  return `${String(d.getHours()).padStart(2, "0")}:${String(d.getMinutes()).padStart(2, "0")}`;
}

/**
 * Helper: Formats timestamp to "DD.MM" string.
 * @param {number} ts - Timestamp.
 * @returns {string} Formatted date string.
 */
function formatDateDDMM(ts) {
  if (ts == null) return "";
  const d = new Date(ts);
  return `${String(d.getDate()).padStart(2, "0")}.${String(d.getMonth() + 1).padStart(2, "0")}`;
}

/**
 * @component
 * @description Renders a scrollable SVG line chart with a gradient fill.
 *
 * @param {Object} props
 * @param {Array<Object>} props.history - Array of historical data objects.
 * @param {string} props.dataKey - The key in the data object to visualize (e.g., 'outdoorTemp').
 * @param {string} [props.color="#4f8cff"] - Hex color code for the line and points.
 * @param {string} [props.title] - Optional title displayed above the chart.
 * @param {string} props.gradientId - Unique ID for the SVG gradient definition (must be unique per chart instance).
 *
 * @returns {JSX.Element|null} The chart component or null if there is insufficient data (< 2 points).
 */
export default function HistoryChart({ 
  history, 
  dataKey, 
  color = "#4f8cff", 
  title, 
  gradientId 
}) {
  const scrollRef = useRef(null);
  
  // 1. Memoize sorted data to prevent unnecessary re-sorting on every render
  const ordered = useMemo(() => sortByTs(history || []), [history]);
  
  // 2. Map data to a simplified structure { ts, val } based on the dynamic dataKey
  const points = useMemo(() => {
    return ordered.map(m => ({
      ts: getTs(m),
      val: m[dataKey] // e.g. m.outdoorTemp or m.indoorTemp
    })).filter(p => p.ts != null && p.val != null);
  }, [ordered, dataKey]);

  // Effect: Automatically scroll the container to the far right (newest data) when points change
  useEffect(() => {
    if (scrollRef.current) {
      scrollRef.current.scrollLeft = scrollRef.current.scrollWidth;
    }
  }, [points]);

  if (points.length < 2) return null;

  // --- Chart Scaling Logic ---
  const values = points.map(p => p.val);
  const minV = Math.min(...values) - 1; // Add buffer to bottom
  const maxV = Math.max(...values) + 1; // Add buffer to top
  const range = maxV - minV || 1;

  const pointSpacing = 70; // Horizontal pixels per data point
  const height = 260;
  const paddingSide = 50;
  const paddingTopBottom = 60;
  const chartWidth = Math.max(points.length * pointSpacing, 800); 
  const width = chartWidth + paddingSide * 2;
  const chartHeight = height - paddingTopBottom * 2;

  // Coordinate mapping functions
  const getX = (i) => paddingSide + (i * (chartWidth / (points.length - 1)));
  const getY = (v) => height - paddingTopBottom - ((v - minV) / range) * chartHeight;

  // --- SVG Path Generation ---
  // Line path
  const pathData = points.reduce((acc, p, i) => 
    `${acc} ${i === 0 ? 'M' : 'L'} ${getX(i)} ${getY(p.val)}`, "");
  
  // Area path (closed loop for gradient fill)
  const areaData = `${pathData} L ${getX(points.length - 1)} ${height - paddingTopBottom} L ${paddingSide} ${height - paddingTopBottom} Z`;

  return (
    <div style={{ marginBottom: "30px" }}>
      {title && (
        <h3 style={{ fontSize: "14px", fontWeight: "600", opacity: 0.8, marginBottom: "12px", color: "#fff" }}>
          {title}
        </h3>
      )}
      
      <div 
        ref={scrollRef}
        style={{ 
          width: "100%", 
          overflowX: "auto", 
          cursor: "grab",
          WebkitOverflowScrolling: "touch",
          scrollbarWidth: "thin",
          scrollbarColor: `${color}33 transparent`, // Hex color + alpha for scrollbar
          paddingBottom: "10px"
        }}
      >
        <svg viewBox={`0 0 ${width} ${height}`} style={{ width: `${width}px`, height: `${height}px`, display: "block" }}>
          <defs>
            <linearGradient id={gradientId} x1="0" y1="0" x2="0" y2="1">
              <stop offset="0%" stopColor={color} stopOpacity="0.4" />
              <stop offset="100%" stopColor={color} stopOpacity="0" />
            </linearGradient>
          </defs>
          
          {/* Baseline (lowest value) */}
          <line x1={0} y1={getY(minV)} x2={width} y2={getY(minV)} stroke="rgba(255,255,255,0.05)" />
          
          <path d={areaData} fill={`url(#${gradientId})`} />
          <path d={pathData} fill="none" stroke={color} strokeWidth="3" strokeLinecap="round" strokeLinejoin="round" />

          {points.map((p, i) => {
            const dateObj = new Date(p.ts);
            const prevDateObj = i > 0 ? new Date(points[i - 1].ts) : null;
            // Show date label only when the day changes
            const showDate = i === 0 || (dateObj.getDate() !== prevDateObj?.getDate());

            return (
              <g key={i}>
                {showDate && i !== 0 && (
                  <line x1={getX(i)} y1={paddingTopBottom} x2={getX(i)} y2={height - paddingTopBottom} stroke="rgba(255,255,255,0.1)" strokeDasharray="4" />
                )}
                
                <circle cx={getX(i)} cy={getY(p.val)} r="4" fill={color} />
                
                {/* Value Label */}
                <text x={getX(i)} y={getY(p.val) - 15} fill="#fff" fontSize="13" textAnchor="middle" fontWeight="bold">
                  {p.val.toFixed(1)}°
                </text>
                
                {/* Time Label */}
                <text x={getX(i)} y={height - 35} fill="rgba(255,255,255,0.5)" fontSize="11" textAnchor="middle">
                  {formatTimeHHMM(p.ts)}
                </text>
                
                {/* Date Label (Conditional) */}
                {showDate && (
                  <text x={getX(i)} y={height - 15} fill={color} fontSize="12" textAnchor="middle" fontWeight="bold">
                    {formatDateDDMM(p.ts)}
                  </text>
                )}
              </g>
            );
          })}
        </svg>
      </div>
    </div>
  );
}