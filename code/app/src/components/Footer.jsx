/**
 * @file Footer.jsx
 * @description The global application footer component.
 * Displays a link to the project's GitHub repository.
 */

/**
 * @component
 * @description Renders the footer section with a GitHub link.
 *
 * @param {Object} props
 * @param {string} [props.githubUrl] - URL to the GitHub repository. Defaults to the author's repo.
 *
 * @returns {JSX.Element} The footer element.
 */
export default function Footer({
  githubUrl = "https://github.com/LukaszWolf?tab=repositories", 
                 
}) {
  return (
    <footer className="app-footer">
      <a
        className="gh"
        href={githubUrl}
        target="_blank"
        rel="noopener noreferrer"
        aria-label="GitHub"
        title="GitHub"
      >
        <svg viewBox="0 0 24 24" width="22" height="22" aria-hidden="true">
          <path fill="currentColor"
            d="M12 .5A11.5 11.5 0 0 0 .5 12c0 5.08 3.29 9.38 7.86 10.9.58.1.79-.25.79-.56v-2.02c-3.2.7-3.87-1.37-3.87-1.37-.53-1.34-1.29-1.7-1.29-1.7-1.06-.73.08-.72.08-.72 1.18.08 1.81 1.22 1.81 1.22 1.04 1.8 2.73 1.28 3.4.98.1-.76.41-1.28.75-1.58-2.55-.29-5.23-1.28-5.23-5.68 0-1.26.45-2.3 1.2-3.11-.12-.3-.52-1.52.11-3.17 0 0 .98-.31 3.21 1.19a11.1 11.1 0 0 1 5.85 0c2.23-1.5 3.21-1.19 3.21-1.19.63 1.65.23 2.87.11 3.17.75.81 1.2 1.85 1.2 3.11 0 4.41-2.69 5.39-5.25 5.67.42.36.8 1.07.8 2.16v3.2c0 .31.21.67.8.56A11.5 11.5 0 0 0 23.5 12 11.5 11.5 0 0 0 12 .5Z"/>
        </svg>
        <span>GitHub</span>
      </a>

    </footer>
  );
}