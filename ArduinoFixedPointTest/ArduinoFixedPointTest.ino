#include "tests.h"

#include "lib/taylormath.cpp"

void setup() {
	Serial.begin(115200); 
  delay(5000);
  Serial.println("\nHELLO THERE\n\n");
}

void loop() {
  Serial.println("\nSTARTING TESTS\n\n");

  Serial.print("fixed32\t");
  all_tests_taylor<fixed32>(100000);

  Serial.print("fixed64\t");
  all_tests_taylor<fixed64>(100000);

  Serial.print("float\t");
  all_tests_std<float>(100000);

  Serial.print("double\t");
  all_tests_std<double>(100000);

  Serial.println("\nDONE\n\n");
  delay(10000);
}

