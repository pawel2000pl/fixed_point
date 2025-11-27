#include "tests.h"

#include "lib/taylormath.cpp"

void setup() {
	Serial.begin(115200); 
  delay(5000);
  Serial.println("\nHELLO THERE\n\n");
}

void loop() {
  unsigned n = 100000;

  Serial.println("\nSTARTING TESTS\n\n");

  Serial.println("<table><thead><tr><th>type</th><th>library</th><th>addition</th><th>subtraction</th><th>multiplication</th><th>division</th><th>sin</th><th>sqrt</th><th>asin</th><th>log</th><th>exp</th></tr></thead><tbody>");

  Serial.print("<tr><th>fixed32_s</th><th>taylormath</th>");
  all_tests_taylor<fixed32_s>(n);
  Serial.println("</tr>");

  Serial.print("<tr><th>fixed32_a</th><th>taylormath</th>");
  all_tests_taylor<fixed32_a>(n);
  Serial.println("</tr>");

  Serial.print("<tr><th>fixed64</th><th>taylormath</th>");
  all_tests_taylor<fixed64>(n);
  Serial.println("</tr>");

  Serial.print("<tr><th>fixed32_s</th><th>polyapprox</th>");
  all_tests_polyapprox<fixed32_s>(n);
  Serial.println("</tr>");

  Serial.print("<tr><th>fixed32_a</th><th>polyapprox</th>");
  all_tests_polyapprox<fixed32_a>(n);
  Serial.println("</tr>");

  Serial.print("<tr><th>fixed64</th><th>polyapprox</th>");
  all_tests_polyapprox<fixed64>(n);
  Serial.println("</tr>");

  Serial.print("<tr><th>float</th><th>cmath</th>");
  all_tests_std<float>(n);
  Serial.println("</tr>");

  Serial.print("<tr><th>double</th><th>cmath</th>");
  all_tests_std<double>(n);
  Serial.println("</tr>");

  Serial.print("<tr><th>float</th><th>taylormath</th>");
  all_tests_taylor<float>(n);
  Serial.println("</tr>");

  Serial.print("<tr><th>double</th><th>taylormath</th>");
  all_tests_taylor<double>(n);
  Serial.println("</tr>");

  Serial.println("</tbody></table>");

  Serial.println("\nDONE\n\n");
  delay(10000);
}

