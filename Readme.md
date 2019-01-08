# jenkins-control

## Circuit

The suitcased consists of switches and LEDs only, its pretty simple:

### Switch / Push Buttons:
 - GPIO Input should be grounded via 1.5k Ohm
 - GPIO Input enable should be 3.3V via 1k Ohm

 ```
       |
       |       \
  GPIO |--+--o  o ---[1k Ohm]--- 3v3
  (in) |  |
 ______|  |
        __|___
       |      |
       | 1.5k |
       |______|
          |
         GND
 ```
 
### Feedback LEDs:
 
 All LEDs should be connected to 3v3 via 120 Ohm resistors
 
```
       |
       |
  GPIO |-----[110 Ohm]---[LED]--- GND
  (out)|
 ______|
```

# Preferred IDE

IntelliJ

# Pinout

## Input Switches

- GPIO 34 (input only) - connected to Protected switch #2
- GPIO 35 (input only) - connected to Protected switch #1
- GPIO 36 (input only) - connected to Protected switch #3
- GPIO 39 - connected to Push Button
- GPIO 32 - connected to Key Switch

## LEDs

- GPIO 33 - Network
- GPIO 25 - Unlocked
- GPIO 26 - Build Green
- GPIO 23 - Build Red
- GPIO 22 - Deploy Red
- GPIO 1  - Deploy Green
- GPIO 3  - Switch #1
- GPIO 18 - Switch #2
- GPIO 5  - Swicth #3
- GPIO 17 - Push Button

# Configuration

Execute `make` and set WIFI / Jenkins properties.

If properties are already set in sdkconfig, do ``make menuconfig``, then navigate to `Jenkins Trigger Configuration` section.

## HTTPS configuration

Run the following command:

```
$ openssl s_client -showcerts -connect www.howsmyssl.com:443 </dev/null
```

and place the Root CA cert into `main/server_root_cert.pem`

Go to `main/include/jenkins.h` and define `JENKINS_ON_HTTPS`.


## Optional
- Set HTTP request timeout in `main/include/http.h` in case your Jenkins instance is slow (default is 5 seconds)
- Set Retry Count and Interval in `main/include/jenkins.h` in case jobs are spending too much time in the Queue.
  Look for `RETRY_JOB_NUMBER` and `DELAY_BEFORE_RETRY`.

Then deploy application to ESP32 device

```
make flash

```