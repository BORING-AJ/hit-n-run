
// =====================================================
//        AUTONOMOUS IR HIT-AND-TURN CAR
//        + WI-FI + RANDOM MP3 AUDIO
//        + WAIT FOR AUDIO TO FINISH
// =====================================================
//
// SEQUENCE:
//
// FORWARD
//    ↓
// IR detects obstacle
//    ↓
// STOP
//    ↓
// PHONE PLAYS RANDOM MP3
//    ↓
// WAIT UNTIL MP3 FINISHES
//    ↓
// PHONE TELLS ESP8266 "AUDIO FINISHED"
//    ↓
// REVERSE
//    ↓
// TURN LEFT
//    ↓
// FORWARD
//    ↓
// WAIT FOR NEXT OBSTACLE
//
// =====================================================
//
// Wi-Fi:
//
// SSID:     IR_CAR
// Password: 12345678
//
// Phone:
//
// Connect to IR_CAR
// Open:
//
// 192.168.4.1
//
// =====================================================
//
// HARDWARE
//
// NodeMCU V3 / ESP8266
//
// L293D:
// IN1 → D1
// IN2 → D2
// IN3 → D5
// IN4 → D6
//
// IR sensor:
// OUT/S → D3
//
// =====================================================


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


// =====================================================
//                    WI-FI
// =====================================================

const char* WIFI_SSID = "IR_CAR";

const char* WIFI_PASSWORD = "12345678";

ESP8266WebServer server(80);


// =====================================================
//                    L293D PINS
// =====================================================

#define IN1 D1
#define IN2 D2
#define IN3 D5
#define IN4 D6


// =====================================================
//                    IR SENSOR
// =====================================================

#define IR_SENSOR D3

#define OBSTACLE_DETECTED LOW


// =====================================================
//                 MOVEMENT TIMES
// =====================================================

const unsigned long REVERSE_TIME = 500;

const unsigned long TURN_TIME = 250;


// =====================================================
//                    AUTO STATES
// =====================================================
//
// IMPORTANT:
//
// STOP_STATE now waits indefinitely.
//
// The car does NOT start reversing based on a timer.
//
// It waits until the PHONE tells the ESP8266 that
// the audio has completely finished.
//
// =====================================================

enum AutoState
{
  FORWARD_STATE,
  STOP_STATE,
  REVERSE_STATE,
  TURN_LEFT_STATE
};


AutoState state = FORWARD_STATE;

unsigned long stateStartTime = 0;


// =====================================================
//              OBSTACLE COUNTER
// =====================================================

unsigned long obstacleCount = 0;


// =====================================================
//                 AUDIO WAIT FLAG
// =====================================================
//
// true:
//
// The car is waiting for the phone's audio to finish.
//
// false:
//
// The car is free to continue.
//
// =====================================================

bool waitingForAudio = false;


// =====================================================
//                    DIRECTIONS
// =====================================================

enum Direction
{
  STOPPED,
  FORWARD,
  BACKWARD,
  LEFT
};


// =====================================================
//              SET MOTOR DIRECTION
// =====================================================

void setMotorDirection(Direction direction)
{

  switch (direction)
  {

    // =================================================
    // FORWARD
    // =================================================

    case FORWARD:

      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);

      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);

      break;


    // =================================================
    // BACKWARD
    // =================================================

    case BACKWARD:

      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);

      break;


    // =================================================
    // LEFT TURN
    // =================================================

    case LEFT:

      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);

      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);

      break;


    // =================================================
    // STOP
    // =================================================

    case STOPPED:

      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);

      break;

  }

}


// =====================================================
//                  GET STATE NAME
// =====================================================

String getStateName()
{

  switch (state)
  {

    case FORWARD_STATE:

      return "FORWARD";


    case STOP_STATE:

      return "WAITING FOR AUDIO";


    case REVERSE_STATE:

      return "REVERSE";


    case TURN_LEFT_STATE:

      return "TURN LEFT";

  }


  return "UNKNOWN";

}


// =====================================================
//              CHANGE AUTO STATE
// =====================================================

void changeState(AutoState newState)
{

  state = newState;

  stateStartTime = millis();


  switch (state)
  {

    // =================================================
    // FORWARD
    // =================================================

    case FORWARD_STATE:

      waitingForAudio = false;

      Serial.println(
        "FORWARD"
      );

      setMotorDirection(
        FORWARD
      );

      break;


    // =================================================
    // STOP
    // =================================================

    case STOP_STATE:

      Serial.println(
        "OBSTACLE DETECTED -> WAITING FOR AUDIO"
      );

      setMotorDirection(
        STOPPED
      );

      break;


    // =================================================
    // REVERSE
    // =================================================

    case REVERSE_STATE:

      waitingForAudio = false;

      Serial.println(
        "AUDIO FINISHED -> REVERSE"
      );

      setMotorDirection(
        BACKWARD
      );

      break;


    // =================================================
    // TURN LEFT
    // =================================================

    case TURN_LEFT_STATE:

      Serial.println(
        "TURN LEFT"
      );

      setMotorDirection(
        LEFT
      );

      break;

  }

}


// =====================================================
//            AUTONOMOUS CAR CONTROL
// =====================================================

void updateAutonomousCar()
{

  switch (state)
  {

    // =================================================
    // FORWARD
    // =================================================

    case FORWARD_STATE:

      // -----------------------------------------------
      // Check IR sensor
      // -----------------------------------------------

      if (
        digitalRead(IR_SENSOR)
        ==
        OBSTACLE_DETECTED
      )
      {

        Serial.println(
          "IR: OBSTACLE DETECTED!"
        );


        // ---------------------------------------------
        // Increase obstacle counter
        // ---------------------------------------------

        obstacleCount++;


        Serial.print(
          "Obstacle count: "
        );

        Serial.println(
          obstacleCount
        );


        // ---------------------------------------------
        // Tell the phone we are waiting for audio
        // ---------------------------------------------

        waitingForAudio = true;


        // ---------------------------------------------
        // STOP
        // ---------------------------------------------

        changeState(
          STOP_STATE
        );

      }

      break;


    // =================================================
    // STOP / WAIT FOR AUDIO
    // =================================================
    //
    // IMPORTANT:
    //
    // There is NO timer here.
    //
    // The car stays stopped until the phone calls:
    //
    // /audioDone
    //
    // =================================================

    case STOP_STATE:

      // Do absolutely nothing.
      //
      // The phone controls when the car continues.

      break;


    // =================================================
    // REVERSE
    // =================================================

    case REVERSE_STATE:

      if (
        millis() - stateStartTime
        >=
        REVERSE_TIME
      )
      {

        changeState(
          TURN_LEFT_STATE
        );

      }

      break;


    // =================================================
    // TURN LEFT
    // =================================================

    case TURN_LEFT_STATE:

      if (
        millis() - stateStartTime
        >=
        TURN_TIME
      )
      {

        changeState(
          FORWARD_STATE
        );

      }

      break;

  }

}


// =====================================================
//              SEND STATUS TO PHONE
// =====================================================

void handleStatus()
{

  String json = "{";


  json += "\"state\":\"";

  json += getStateName();

  json += "\",";


  json += "\"obstacles\":";

  json += String(obstacleCount);


  json += ",";


  json += "\"waiting\":";

  if (waitingForAudio)
  {
    json += "true";
  }
  else
  {
    json += "false";
  }


  json += "}";


  server.send(
    200,
    "application/json",
    json
  );

}


// =====================================================
//              AUDIO FINISHED
// =====================================================
//
// The PHONE calls this URL when the MP3 reaches
// its "ended" event.
//
// Example:
//
// http://192.168.4.1/audioDone
//
// =====================================================

void handleAudioDone()
{

  Serial.println(
    "PHONE: AUDIO FINISHED"
  );


  // -------------------------------------------------
  // Only respond if the car is actually waiting
  // for audio.
  // -------------------------------------------------

  if (
    state == STOP_STATE &&
    waitingForAudio == true
  )
  {

    Serial.println(
      "AUDIO COMPLETE -> STARTING CAR"
    );


    waitingForAudio = false;


    // ------------------------------------------------
    // Start reverse
    // ------------------------------------------------

    changeState(
      REVERSE_STATE
    );

  }


  server.send(
    200,
    "text/plain",
    "OK"
  );

}


// =====================================================
//                 PHONE WEB PAGE
// =====================================================

void handleRoot()
{

  String html = R"rawliteral(

<!DOCTYPE html>

<html lang="en">

<head>

<meta charset="UTF-8">

<meta
  name="viewport"
  content="width=device-width, initial-scale=1.0"
>

<meta
  name="theme-color"
  content="#111111"
>

<title>IR Autonomous Car</title>


<style>


/* ================================================= */
/*                    PAGE                            */
/* ================================================= */

* {
  box-sizing: border-box;
}


body {

  margin: 0;

  padding: 20px;

  background: #111111;

  color: #ffffff;

  font-family:
    -apple-system,
    BlinkMacSystemFont,
    "Segoe UI",
    Roboto,
    Arial,
    sans-serif;

  text-align: center;

}


/* ================================================= */
/*                    CARD                            */
/* ================================================= */

.container {

  width: 100%;

  max-width: 460px;

  margin: 0 auto;

  padding: 24px;

  background: #202020;

  border-radius: 18px;

  box-shadow:
    0 4px 20px rgba(0,0,0,0.35);

}


/* ================================================= */
/*                    TITLE                           */
/* ================================================= */

.title {

  margin: 0;

  font-size: 25px;

  font-weight: 600;

  letter-spacing: 0.3px;

}


.subtitle {

  margin-top: 8px;

  color: #aaaaaa;

  font-size: 14px;

}


/* ================================================= */
/*                   DIVIDER                          */
/* ================================================= */

.divider {

  height: 1px;

  background: #3a3a3a;

  margin: 22px 0;

}


/* ================================================= */
/*                STATE LABEL                         */
/* ================================================= */

.state-label {

  font-size: 13px;

  color: #999999;

  text-transform: uppercase;

  letter-spacing: 1px;

}


#state {

  margin-top: 8px;

  font-size: 24px;

  font-weight: 600;

  line-height: 1.3;

}


/* ================================================= */
/*              OBSTACLE COUNT                       */
/* ================================================= */

#hit {

  margin-top: 12px;

  font-size: 16px;

  color: #cccccc;

}


/* ================================================= */
/*               AUDIO SECTION                        */
/* ================================================= */

.section-title {

  margin-top: 10px;

  margin-bottom: 14px;

  font-size: 18px;

  font-weight: 600;

}


/* ================================================= */
/*                    BUTTONS                         */
/* ================================================= */

button {

  width: 100%;

  padding: 14px 16px;

  margin: 7px 0;

  border: none;

  border-radius: 10px;

  background: #333333;

  color: #ffffff;

  font-family: inherit;

  font-size: 16px;

  font-weight: 500;

  cursor: pointer;

}


button:active {

  transform: scale(0.98);

  background: #444444;

}


/* ================================================= */
/*                 FILE LIST                          */
/* ================================================= */

#files {

  width: 100%;

  margin-top: 15px;

  padding: 14px;

  background: #151515;

  border: 1px solid #383838;

  border-radius: 10px;

  text-align: left;

  font-size: 14px;

  line-height: 1.7;

  color: #cccccc;

  max-height: 180px;

  overflow-y: auto;

}


/* ================================================= */
/*                SOUND STATUS                        */
/* ================================================= */

#soundStatus {

  margin-top: 14px;

  font-size: 14px;

  color: #aaaaaa;

  min-height: 20px;

}


/* ================================================= */
/*               HIDDEN INPUTS                        */
/* ================================================= */

input[type="file"] {

  display: none;

}


/* ================================================= */
/*                    FOOTER                          */
/* ================================================= */

.footer {

  margin-top: 20px;

  font-size: 12px;

  color: #777777;

}


</style>

</head>


<body>


<div class="container">


<!-- ================================================= -->
<!--                     TITLE                         -->
<!-- ================================================= -->

<h1 class="title">
  IR Autonomous Car
</h1>


<div class="subtitle">
  Wi-Fi obstacle detection system
</div>


<div class="divider"></div>


<!-- ================================================= -->
<!--                  CAR STATUS                       -->
<!-- ================================================= -->

<div class="state-label">
  Current state
</div>


<div id="state">
  Connecting...
</div>


<div id="hit">
  Obstacles detected: 0
</div>


<div class="divider"></div>


<!-- ================================================= -->
<!--                 AUDIO SECTION                     -->
<!-- ================================================= -->

<div class="section-title">
  Audio Collection
</div>


<button onclick="chooseFolder()">
  Choose Audio Folder
</button>


<button onclick="chooseFiles()">
  Choose MP3 Files
</button>


<!-- ================================================= -->
<!--                  FOLDER PICKER                    -->
<!-- ================================================= -->

<input
  type="file"
  id="folderPicker"
  webkitdirectory
  directory
  multiple
  accept="audio/*"
>


<!-- ================================================= -->
<!--                  FILE PICKER                      -->
<!-- ================================================= -->

<input
  type="file"
  id="filePicker"
  multiple
  accept="audio/*"
>


<!-- ================================================= -->
<!--                  FILE LIST                        -->
<!-- ================================================= -->

<div id="files">
  No audio files selected
</div>


<!-- ================================================= -->
<!--                 ENABLE SOUND                      -->
<!-- ================================================= -->

<button onclick="enableSound()">
  Enable Sound
</button>


<div id="soundStatus">
  Sound disabled
</div>


<!-- ================================================= -->
<!--                  AUDIO PLAYER                     -->
<!-- ================================================= -->

<audio id="player"></audio>


<div class="footer">
  Connect to IR_CAR and keep this page open
</div>


</div>


<script>


// =================================================
//                  VARIABLES
// =================================================

let lastObstacleCount = 0;

let soundEnabled = false;

let audioCollection = [];

let lastPlayedIndex = -1;


// =================================================
//               CHOOSE AUDIO FOLDER
// =================================================

function chooseFolder()
{

  document
    .getElementById("folderPicker")
    .click();

}


// =================================================
//              FOLDER SELECTED
// =================================================

document
  .getElementById("folderPicker")
  .addEventListener(
    "change",
    function()
    {

      let files =
        Array.from(this.files);


      audioCollection =
        files.filter(
          function(file)
          {

            return (
              file.type.startsWith("audio/")
              ||
              /\.(mp3|wav|ogg|m4a)$/i
                .test(file.name)
            );

          }
        );


      updateFileList();

    }
  );


// =================================================
//              CHOOSE INDIVIDUAL FILES
// =================================================

function chooseFiles()
{

  document
    .getElementById("filePicker")
    .click();

}


// =================================================
//               FILES SELECTED
// =================================================

document
  .getElementById("filePicker")
  .addEventListener(
    "change",
    function()
    {

      audioCollection =
        Array.from(this.files)
        .filter(
          function(file)
          {

            return (
              file.type.startsWith("audio/")
              ||
              /\.(mp3|wav|ogg|m4a)$/i
                .test(file.name)
            );

          }
        );


      updateFileList();

    }
  );


// =================================================
//                UPDATE FILE LIST
// =================================================

function updateFileList()
{

  let fileList =
    document.getElementById("files");


  if (
    audioCollection.length === 0
  )
  {

    fileList.innerHTML =
      "No audio files found";


    document
      .getElementById("soundStatus")
      .innerHTML =
        "No audio files selected";


    return;

  }


  let html =
    "<strong>" +
    audioCollection.length +
    " audio files loaded</strong><br><br>";


  audioCollection.forEach(
    function(file, index)
    {

      html +=
        (index + 1) +
        ". " +
        file.name +
        "<br>";

    }
  );


  fileList.innerHTML =
    html;


  document
    .getElementById("soundStatus")
    .innerHTML =
      "Audio collection ready";

}


// =================================================
//                 ENABLE SOUND
// =================================================

function enableSound()
{

  if (
    audioCollection.length === 0
  )
  {

    document
      .getElementById("soundStatus")
      .innerHTML =
        "Select an audio folder first";

    return;

  }


  soundEnabled = true;


  document
    .getElementById("soundStatus")
    .innerHTML =
      "Sound enabled";

}


// =================================================
//             GET RANDOM AUDIO FILE
// =================================================

function getRandomAudio()
{

  if (
    audioCollection.length === 0
  )
  {

    return null;

  }


  let randomIndex;


  // ------------------------------------------------
  // Only one file
  // ------------------------------------------------

  if (
    audioCollection.length === 1
  )
  {

    randomIndex = 0;

  }


  // ------------------------------------------------
  // Multiple files
  // ------------------------------------------------

  else
  {

    do
    {

      randomIndex =
        Math.floor(
          Math.random() *
          audioCollection.length
        );

    }

    while (
      randomIndex === lastPlayedIndex
    );

  }


  lastPlayedIndex =
    randomIndex;


  return audioCollection[randomIndex];

}


// =====================================================
//                PLAY RANDOM SOUND
// =====================================================
//
// IMPORTANT:
//
// The car is already STOPPED when this happens.
//
// The car will remain stopped until the audio's
// "ended" event occurs.
//
// =====================================================

function playRandomSound()
{

  if (!soundEnabled)
  {

    return;

  }


  if (
    audioCollection.length === 0
  )
  {

    return;

  }


  let selectedFile =
    getRandomAudio();


  if (!selectedFile)
  {

    return;

  }


  let player =
    document.getElementById(
      "player"
    );


  // ------------------------------------------------
  // Stop previous audio
  // ------------------------------------------------

  player.pause();

  player.currentTime = 0;


  // ------------------------------------------------
  // Create temporary URL
  // ------------------------------------------------

  let audioURL =
    URL.createObjectURL(
      selectedFile
    );


  player.src =
    audioURL;


  // ------------------------------------------------
  // Display selected audio
  // ------------------------------------------------

  document
    .getElementById("soundStatus")
    .innerHTML =
      "Playing: " +
      selectedFile.name;


  // ------------------------------------------------
  // PLAY AUDIO
  // ------------------------------------------------

  player.play()
    .then(
      function()
      {

        console.log(
          "Playing:",
          selectedFile.name
        );

      }
    )
    .catch(
      function(error)
      {

        console.log(
          "Audio playback blocked:",
          error
        );


        document
          .getElementById("soundStatus")
          .innerHTML =
            "Audio playback blocked";

      }
    );


  // =================================================
  // AUDIO FINISHED
  // =================================================
  //
  // THIS IS THE IMPORTANT PART.
  //
  // The ESP8266 receives /audioDone ONLY AFTER
  // the audio has completely finished.
  //
  // =================================================

  player.onended =
    function()
    {

      console.log(
        "Audio finished"
      );


      document
        .getElementById("soundStatus")
        .innerHTML =
          "Waiting for next obstacle";


      // ---------------------------------------------
      // Tell ESP8266 that audio is finished
      // ---------------------------------------------

      fetch(
        "/audioDone"
      )
      .then(
        function(response)
        {

          return response.text();

        }
      )
      .then(
        function(data)
        {

          console.log(
            "ESP8266:",
            data
          );

        }
      )
      .catch(
        function(error)
        {

          console.log(
            "Could not notify ESP8266:",
            error
          );

        }
      );


      // ---------------------------------------------
      // Release temporary audio URL
      // ---------------------------------------------

      URL.revokeObjectURL(
        audioURL
      );

    };

}


// =================================================
//                 UPDATE STATUS
// =================================================

function updateStatus()
{

  fetch("/status")

    .then(
      function(response)
      {

        return response.json();

      }
    )

    .then(
      function(data)
      {

        // -----------------------------------------
        // Current state
        // -----------------------------------------

        document
          .getElementById("state")
          .innerHTML =
            data.state;


        // -----------------------------------------
        // Obstacle count
        // -----------------------------------------

        document
          .getElementById("hit")
          .innerHTML =
            "Obstacles detected: " +
            data.obstacles;


        // -----------------------------------------
        // NEW OBSTACLE
        // -----------------------------------------
        //
        // Only play when the obstacle counter
        // increases.
        //
        // -----------------------------------------

        if (
          data.obstacles >
          lastObstacleCount
        )
        {

          playRandomSound();

        }


        // -----------------------------------------
        // Save counter
        // -----------------------------------------

        lastObstacleCount =
          data.obstacles;

      }
    )

    .catch(
      function(error)
      {

        document
          .getElementById("state")
          .innerHTML =
            "Car not connected";

      }
    );

}


// =================================================
//              STATUS UPDATE TIMER
// =================================================

setInterval(
  updateStatus,
  200
);


// =================================================
//             INITIAL STATUS UPDATE
// =================================================

updateStatus();


</script>


</body>

</html>

)rawliteral";


  server.send(
    200,
    "text/html",
    html
  );

}


// =====================================================
//                         SETUP
// =====================================================

void setup()
{

  Serial.begin(115200);


  // =================================================
  // MOTOR PINS
  // =================================================

  pinMode(IN1, OUTPUT);

  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);

  pinMode(IN4, OUTPUT);


  // =================================================
  // IR SENSOR
  // =================================================

  pinMode(
    IR_SENSOR,
    INPUT
  );


  // =================================================
  // INITIAL MOTOR STATE
  // =================================================

  setMotorDirection(
    STOPPED
  );


  // =================================================
  // START WI-FI ACCESS POINT
  // =================================================

  WiFi.mode(
    WIFI_AP
  );


  WiFi.softAP(
    WIFI_SSID,
    WIFI_PASSWORD
  );


  // =================================================
  // WEB SERVER ROUTES
  // =================================================

  server.on(
    "/",
    handleRoot
  );


  server.on(
    "/status",
    handleStatus
  );


  // -------------------------------------------------
  // AUDIO FINISHED ROUTE
  // -------------------------------------------------

  server.on(
    "/audioDone",
    handleAudioDone
  );


  // =================================================
  // START SERVER
  // =================================================

  server.begin();


  // =================================================
  // SERIAL INFORMATION
  // =================================================

  Serial.println();

  Serial.println(
    "================================"
  );

  Serial.println(
    "       IR HIT AND TURN CAR"
  );

  Serial.println(
    "       RANDOM AUDIO SYSTEM"
  );

  Serial.println(
    "       AUDIO WAIT ENABLED"
  );

  Serial.println(
    "================================"
  );


  Serial.println();


  Serial.print(
    "Wi-Fi Network: "
  );

  Serial.println(
    WIFI_SSID
  );


  Serial.print(
    "IP Address: "
  );

  Serial.println(
    WiFi.softAPIP()
  );


  Serial.println();


  Serial.println(
    "Connect phone to IR_CAR"
  );


  Serial.println(
    "Open 192.168.4.1"
  );


  Serial.println();


  Serial.println(
    "Starting in 3 seconds..."
  );


  // =================================================
  // STARTUP DELAY
  // =================================================

  delay(3000);


  // =================================================
  // START FORWARD
  // =================================================

  changeState(
    FORWARD_STATE
  );

}


// =====================================================
//                         LOOP
// =====================================================

void loop()
{

  // -------------------------------------------------
  // Handle Wi-Fi requests
  // -------------------------------------------------

  server.handleClient();


  // -------------------------------------------------
  // Autonomous car
  // -------------------------------------------------

  updateAutonomousCar();

}
