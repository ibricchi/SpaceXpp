/*
   Written by Nicholas Pfaff (nicholas.pfaff19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/

package server

import (
	"bufio"
	"context"
	"fmt"
	"os"

	"golang.org/x/crypto/bcrypt"
)

type credential struct {
	username string
	password string
}

func AddCredential(ctx context.Context, db DB) error {
	quitStr := "#"

	fmt.Println("Adding new user (Enter # to quit):")
	fmt.Println("Enter username: ")
	username, quit := validateCredentialStringInput(quitStr)
	if quit {
		return nil
	}

	// Want to guarantee unique usernames
	creds, err := db.getCredentials(ctx)
	if err != nil {
		return fmt.Errorf("server: auth: failed to get creds: %w", err)
	}
	if _, exists := creds[username]; exists {
		fmt.Println("Username already exists. Please try again.")
		return AddCredential(ctx, db)
	}

	fmt.Println("Enter password: ")
	password, quit := validateCredentialStringInput(quitStr)
	if quit {
		return nil
	}

	// Hash with salt
	bytePassword := []byte(password)
	passwordHash, err := bcrypt.GenerateFromPassword(bytePassword, bcrypt.MinCost)
	if err != nil {
		return fmt.Errorf("server: credentials: failed to encrypt password: %w", err)
	}

	if err := db.insertCredentials(ctx, credential{
		username: username,
		password: string(passwordHash),
	}); err != nil {
		return fmt.Errorf("server: credentials: failed to insert credentials into db: %w", err)
	}

	// keep asking to add more credentials until quit
	return AddCredential(ctx, db)
}

// Returns a 'true' boolean and empty string if quit. Returns a valid user input otherwise.
func validateCredentialStringInput(quitStr string) (string, bool) {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Scan()
	input := scanner.Text()

	if input == quitStr {
		return "", true
	}

	// Don't accept empty string
	if input == "" {
		fmt.Print("A value is required. Please try again: ")
		return validateCredentialStringInput(quitStr)
	}

	// Minimum length requirement
	if len(input) < 3 {
		fmt.Print("Input must contain at least three characters. Please try again: ")
		return validateCredentialStringInput(quitStr)
	}

	return input, false
}
