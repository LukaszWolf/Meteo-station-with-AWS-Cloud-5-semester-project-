/**
 * @file useAuth.js
 * @description Custom hook to manage user authentication state using AWS
 * Amplify. It handles sign-in/sign-out events and ensures the necessary IoT
 * policies are attached to the user identity.
 */

import {Auth, Hub} from 'aws-amplify';
import {useEffect, useState} from 'react';

/** API Endpoint for attaching IoT Policy to Cognito Identity */
const ATTACH_API_URL =
    'https://wjngrfdjy3.execute-api.eu-north-1.amazonaws.com/attach';

/**
 * @function useAuth
 * @description Provides authentication state and methods.
 *
 * @returns {Object} Auth object containing:
 * - user {Object|null}: The current authenticated user object or null.
 * - signIn {Function}: Method to trigger federated sign-in.
 * - signOut {Function}: Method to sign out the current user.
 */
export function useAuth() {
  const [user, setUser] = useState(null);

  /**
   * Calls the backend API to attach the AWS IoT Policy to the current Cognito
   * Identity. This is required for the user to publish/subscribe to MQTT
   * topics.
   */
  const ensureIoTPolicyAttached = async () => {
    try {
      const creds = await Auth.currentCredentials();
      let headers = {'Content-Type': 'application/json'};
      try {
        const session = await Auth.currentSession();
        headers.Authorization = session.getIdToken().getJwtToken();
      } catch {
        // Token might not be available, proceed without it if necessary
      }

      await fetch(ATTACH_API_URL, {
        method: 'POST',
        headers,
        body: JSON.stringify({identityId: creds.identityId}),
      });
    } catch (e) {
      console.warn('Policy attach error:', e);
    }
  };

  /**
   * Checks for an authenticated user session and updates state.
   */
  const checkUser = async () => {
    try {
      const u = await Auth.currentAuthenticatedUser();
      setUser(u);
      await ensureIoTPolicyAttached();
    } catch {
      setUser(null);
    }
  };

  /**
   * Effect: Sets up an Amplify Hub listener to react to auth events (signIn,
   * signOut).
   */
  useEffect(() => {
    const unsub = Hub.listen('auth', ({payload: {event}}) => {
      if (event === 'signIn' || event === 'cognitoHostedUI')
        checkUser();
      else if (event === 'signOut')
        setUser(null);
    });
    checkUser();
    return unsub;
  }, []);

  const signIn = () => Auth.federatedSignIn();
  const signOut = () => Auth.signOut();

  return {user, signIn, signOut};
}