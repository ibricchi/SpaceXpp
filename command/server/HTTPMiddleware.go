/*
   Written by Nicholas Pfaff (nicholas.pfaff19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/

package server

import (
	"fmt"
	"net/http"

	"golang.org/x/crypto/bcrypt"
)

func allowOriginFunc(r *http.Request, origin string) bool {
	// Allow all without checking origin
	return true
}

func (h *HttpServer) basicAuth(realm string, creds map[string]string) func(next http.Handler) http.Handler {
	return func(next http.Handler) http.Handler {
		return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			username, password, ok := r.BasicAuth()
			if !ok {
				h.basicAuthFailed(w, realm, "Authentication failed")
				return
			}

			passwordHash, userExists := creds[username]
			if !userExists {
				h.basicAuthFailed(w, realm, fmt.Sprintf("Invalid username: %v", username))
				return
			}

			passwordHashBytes := []byte(passwordHash)
			passwordBytes := []byte(password)
			if err := bcrypt.CompareHashAndPassword(passwordHashBytes, passwordBytes); err != nil {
				h.basicAuthFailed(w, realm, "Incorrect password")
				return
			}

			// Authorised
			next.ServeHTTP(w, r)
		})
	}
}

func (h *HttpServer) basicAuthFailed(w http.ResponseWriter, realm string, msg string) {
	w.Header().Add("WWW-Authenticate", fmt.Sprintf(`Basic realm="%s"`, realm))
	http.Error(w, msg, http.StatusUnauthorized)

	h.logger.Info("HTTP auth failed: " + msg)
}
