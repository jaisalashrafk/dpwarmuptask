# dpwarmuptask

Using the sensor data effectively: acquiring the data, processing it to calculate the
energy of motion, normalizing the energy for easy interpretation, and doing all of this in
real time. All of these components together make the system versatile for a number of
potential applications

#Algorithm
1. Initialize I2C and configure the GPIO pins.
2. Initiate communication with LSM6DS3 using I2C.
3. Obtain data from LSM6DS3.
4. Create a window of data for a certain length (the frame).
5. Perform FFT on the window of data using a library like esp-dsp.
6. Calculate the energy by squaring the absolute value of the FFT result.
7. Normalize the energy by dividing by the square of the number of frames.
8. Store normalized energy and move the window over the recorded data.
9. Repeat steps 4 to 8 until end of data.
