# esp32-jenkins-trigger-public

ESP32 sources for the following project: 

https://hackaday.io/project/162839-jenkins-build-deploy-launch-case

## Circuit

The suitcase consists of switches and LEDs only, its pretty simple:

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
 
 All LEDs should be connected to 3v3 via 110 Ohm resistors
 
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

```
+---------+--------------+---------------------+
| GPIO    | Nature       | Action              |
+---------+--------------+---------------------+
| 34      | input only   | Protected Switch #2 |                     
+---------+--------------+---------------------+
| 35      | input only   | Protected Switch #1 |                     
+---------+--------------+---------------------+
| 36      | input only   | Protected Switch #3 |                     
+---------+--------------+---------------------+
| 39      |              | Red Push Button     |                     
+---------+--------------+---------------------+
| 32      |              | Key Switch          |                     
+---------+--------------+---------------------+
```

## LEDs

```
+---------+----------------------+
| GPIO    | Displays             |
+---------+----------------------+
| 33      | Network status       |                     
+---------+----------------------+
| 25      | Authorization status |                     
+---------+----------------------+
| 26      | Build                |                     
+---------+----------------------+
| 23      | Build Failed         |                     
+---------+----------------------+
| 22      | Deploy Failed        |                     
+---------+----------------------+
| 1       | Deploy               |                     
+---------+----------------------+
| 3       | Component Switch #1  |                     
+---------+----------------------+
| 18      | Component Switch #2  |                     
+---------+----------------------+
| 5       | Component Switch #3  |                     
+---------+----------------------+
| 17      | Start Button         |                     
+---------+----------------------+
```

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