/**
 * @file useStationClaim.js
 * @description Custom hook responsible for the "Claiming" (Pairing) process.
 * It sends a claim request via API Gateway and listens for a confirmation
 * message via MQTT.
 */

import {Auth, PubSub} from 'aws-amplify';
import {useState} from 'react';

/** API Gateway endpoint for the Claim Lambda function */
const CLAIM_API_URL =
    'https://rp6817rcg4.execute-api.eu-north-1.amazonaws.com/claim/reply';

/**
 * @function useStationClaim
 * @description Manages the state and logic for claiming a new device.
 *
 * @param {Object} user - The authenticated user.
 * @param {Function} onSuccess - Callback function to execute after successful
 *     claiming (e.g., reload data).
 *
 * @returns {Object} Claim hook object containing:
 * - thing {string}: Current input value for Thing Name.
 * - setThing {Function}: Setter for Thing Name.
 * - nonce {string}: Current input value for Nonce code.
 * - setNonce {Function}: Setter for Nonce code.
 * - claimStatus {string}: Status message.
 * - handleClaim {Function}: Function to initiate the claim process.
 */
export function useStationClaim(user, onSuccess) {
  const [thing, setThing] = useState('');
  const [nonce, setNonce] = useState('');
  const [claimStatus, setClaimStatus] = useState('');

  const handleClaim = async () => {
    try {
      if (!user) {
        setClaimStatus('Zaloguj się najpierw.');
        return;
      }
      if (!thing || !nonce) {
        setClaimStatus('Wypełnij oba pola.');
        return;
      }

      setClaimStatus('Wysyłam kod...');
      const dataTopic = `devices/${thing}/data`;
      let isConfirmed = false;

      // 1. Subscribe to the device's data topic to listen for the first message
      const subscription = PubSub.subscribe(dataTopic).subscribe({
        next: (msg) => {
          isConfirmed = true;
          setClaimStatus('Sparowano pomyślnie!');
          subscription.unsubscribe();
          if (onSuccess) onSuccess();  // Refresh dashboard data
        },
        error: (e) => console.error(e),
      });

      // 2. Send the claim request to the API
      const creds = await Auth.currentCredentials();
      let headers = {'Content-Type': 'application/json'};
      try {
        const session = await Auth.currentSession();
        headers.Authorization = session.getIdToken().getJwtToken();
      } catch {
        // Session might be invalid or expired
      }

      await fetch(CLAIM_API_URL, {
        method: 'POST',
        headers,
        body: JSON.stringify(
            {thingName: thing, identityId: creds.identityId, nonce}),
      });

      // 3. Set a timeout to handle cases where no data is received
      setTimeout(() => {
        if (!isConfirmed) {
          subscription.unsubscribe();
          if (claimStatus !== 'Sparowano pomyślnie!') {
            setClaimStatus('Wysłano. Czekam na dane...');
          }
        }
      }, 15000);

    } catch (e) {
      setClaimStatus('Błąd: ' + e.message);
    }
  };

  return {thing, setThing, nonce, setNonce, claimStatus, handleClaim};
}