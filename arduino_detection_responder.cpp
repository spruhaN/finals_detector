#if defined(ARDUINO) && !defined(ARDUINO_ARDUINO_NANO33BLE)
#define ARDUINO_EXCLUDE_CODE
#endif  // defined(ARDUINO) && !defined(ARDUINO_ARDUINO_NANO33BLE)

#ifndef ARDUINO_EXCLUDE_CODE

#include "detection_responder.h"

#include "Arduino.h"

// LEDs are reversed!
// flashes the blue LED after each inference, aggregate scores every second.
void RespondToDetection(tflite::ErrorReporter* error_reporter,
                        int8_t happy_score, int8_t sad_score) {
  static bool is_initialized = false;
  static unsigned long last_aggregate_time = 0;
  static int aggregate_happy_score = 0;
  static int aggregate_sad_score = 0;
  static int inference_count = 0;

  if (!is_initialized) {
    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);
    pinMode(D12, OUTPUT); // TODO:pin for taser
    is_initialized = true;
  }

  // flashes the blue LED after every inference.
  digitalWrite(LEDB, LOW);
  delay(100);
  digitalWrite(LEDB, HIGH);

  // sum scores
  aggregate_happy_score += happy_score;
  aggregate_sad_score += sad_score;
  inference_count++;

  unsigned long current_time = millis();

  if (current_time - last_aggregate_time >= 5000) { // every 5 seconds
    // avg
    int average_happy_score = aggregate_happy_score / inference_count;
    int average_sad_score = aggregate_sad_score / inference_count;

    // reset
    aggregate_happy_score = 0;
    aggregate_sad_score = 0;
    inference_count = 0;
    last_aggregate_time = current_time;

    // LEDS off
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDR, HIGH);


    if (average_happy_score > average_sad_score) { // green is happy
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDR, HIGH);
    } else { // red is sad
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDR, LOW);
      // taser
      digitalWrite(D12, HIGH);
      delay(1000); // 1 sec
      digitalWrite(D12, LOW);
    }

    TF_LITE_REPORT_ERROR(error_reporter, "Avg Happy score: %d Avg Sad score: %d",
                         average_happy_score, average_sad_score);
  }
}

#endif  // ARDUINO_EXCLUDE_CODE
