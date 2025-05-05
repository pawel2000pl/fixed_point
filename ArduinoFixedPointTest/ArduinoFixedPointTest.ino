#include "tests.h"

#include "lib/taylormath.cpp"

void setup() {
	Serial.begin(115200); 
  delay(5000);
  Serial.println("\nHELLO THERE\n\n");
}

void loop() {
  Serial.println("\nSTARTING TESTS\n\n");

  Serial.println("<table><thead><th><th>addition</th><th>subtraction</th><th>multiplication</th><th>division</th><th>sin</th><th>sqrt</th><th>asin</th><th>log</th><th>exp</th></tr></thead><tbody>");
  
  Serial.print("<tr><th>fixed32_s</th>");
  all_tests_taylor<fixed32_s>(100000);
  Serial.println("</tr>");

  Serial.print("<tr><th>fixed32_a</th>");
  all_tests_taylor<fixed32_a>(100000);
  Serial.println("</tr>");

  Serial.print("<tr><th>fixed64</th>");
  all_tests_taylor<fixed64>(100000);
  Serial.println("</tr>");

  Serial.print("<tr><th>float</th>");
  all_tests_std<float>(100000);
  Serial.println("</tr>");

  Serial.print("<tr><th>double</th>");
  all_tests_std<double>(100000);
  Serial.println("</tr>");

  Serial.println("</tbody></table>");

  Serial.println("\nDONE\n\n");
  delay(10000);
}

