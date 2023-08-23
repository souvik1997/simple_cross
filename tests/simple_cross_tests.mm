//
//  simple_cross_tests.m
//  simple_cross_tests
//
//  Created by Souvik Banerjee on 8/22/23.
//

#import <XCTest/XCTest.h>
#include <fstream>
#include <iostream>

#include "simple_cross.hpp"

@interface simple_cross_tests : XCTestCase
@end

@implementation simple_cross_tests

- (void)testInput
{
    /* iterate over all test cases */
    size_t i = 1;
    while (true) {
        /* find input and output files */
        NSString* inputFile = [NSString stringWithFormat:@"input_%zu", i];
        NSString* inputFilePath = [[NSBundle bundleForClass:[simple_cross_tests class]] pathForResource:inputFile ofType:@"txt"];
        NSString* outputFile = [NSString stringWithFormat:@"output_%zu", i];
        NSString* outputFilePath = [[NSBundle bundleForClass:[simple_cross_tests class]] pathForResource:outputFile ofType:@"txt"];
        if (inputFilePath == nil || outputFilePath == nil) {
            break;
        }
        SimpleCross scross;
        std::vector<std::string> result;

        /* read input */
        auto actions = std::ifstream(inputFilePath.UTF8String);

        std::string line;
        while (std::getline(actions, line)) {
            /* cross input */
            results_t results = scross.action(line);
            /* collect test output */
            result.insert(result.end(), results.begin(), results.end());
        }

        /* read expected output */
        auto outputStream = std::ifstream(outputFilePath.UTF8String);
        std::vector<std::string> output;
        std::string outputLine;
        while (std::getline(outputStream, outputLine)) {
            output.push_back(outputLine);
        }

        /* assert test output is expected output */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-statement-expression"
        XCTAssertTrue(result == output, "Test %@ failed", inputFile);
#pragma clang diagnostic pop
        i++;
    }
}

@end
