# Midterm Project

## Simon Game Requirements:
- When buttons are pressed, do LEDS light up as expected/ do proper sounds come out?
- Does it time out if the user takes too long to press a button?
- Does it gameover if the user presses the wrong button?
- Does it have a light/sound sequence for wins/losses?
- Is the sequence random?
- Is it easy to change the number of rounds?
- Comments, style, etc
- Terminal should output the current round and color pressed as its being pressed, then stop when the button is released.
- Play some light/sound animation on boot

## Pinout:
- Red Button: P2.0
- Green Button: P2.2
- Blue Button: P2.3
- Yellow Button: P2.4
- Piezo Buzzer: P2.5
- LED Strip should be configured to UCB0SIMO (P1.6) and UCB0CLK (P1.5)
- Configure UCA0 to UART mode for the terminal output.

## Extra Credit:
5 points for some interesting additional feature that is not listed above.
ideas:
- Piano Tiles?
    - The LED strip lights up a random color and the user has to press the corresponding button as fast as possible.
    - Returns the players score in the terminal.
    - if the player presses the wrong button, the game ends.

## Schedule:
**DUE DATE: 3/27/2024 at 11:59 PM**
- Pre-Exams: 
    - Compile existing code and make sure it works as expected.
        - Have all code in GitHub Repo for reference later by Wednesday 3/20/2024
    - put hardware together and make sure it works as expected.
- After Exams:
    - Put code together, squash bugs, and make sure it works as expected.
    - test, make sure comments are there and it makes sense


