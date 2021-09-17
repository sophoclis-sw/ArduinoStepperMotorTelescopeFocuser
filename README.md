- This project is DIY Arduino telescope focuser.
- The Arduino drives a 28YBT-48 DC 5V stepper motor, using a ULN2003 driver.
- Using 2 push buttons, you can select 5 focusing speeds, increasing every time you press the same button and decreasing as you press the other one.
- Pressing both buttons simultaneously stops the focusing motor.
- It has an accuracy of about 1/10 of a degree, because the stepper motor has a 64x gearbox, so it takes a total of 4096 steps to complete a 360º.
- The software also supports an infrared remote control instead of the buttons for the same functionality.
- For the mounting, of the stepper motor, I used a spare L-bracket from a PC case, and the Arduino with the ULN2003 driver was fitted inside the arm of the telescope as in the photo, with double sided adhesive tape. 
- The power for the Arduino & the stepper motor was taken after the 5V regulator of the telescope.


<!---
skesinis/skesinis is a ✨ special ✨ repository because its `README.md` (this file) appears on your GitHub profile.
You can click the Preview link to take a look at your changes.
--->
