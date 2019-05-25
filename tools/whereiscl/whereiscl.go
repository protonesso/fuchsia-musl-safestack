// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// whereiscl is a command-line utility that answers "Where is my CL?".
// Given a Gerrit review URL, it will answer
//   - Whether the CL was merged (or abandoned)
//   - Whether the CL passed Global Integration (if merged)
package main

import (
	"errors"
	"flag"
	"fmt"
	"log"
	"os"
	"regexp"

	"fuchsia.googlesource.com/fuchsia/tools/whereiscl/lib"
)

func init() {
	flag.Usage = func() {
		fmt.Fprintf(flag.CommandLine.Output(), `Usage: whereiscl <review URL>

Answers whether a CL is merged (or abandoned) and whether it passed Global Integration.

Examples:
  $ whereiscl https://fuchsia-review.googlesource.com/c/fuchsia/+/123456789
  $ whereiscl fuchsia-review.googlesource.com/c/fuchsia/+/123456789/some/file
  $ whereiscl http://fxr/123456789
  $ whereiscl fxr/123456789/some/file

`)
		flag.PrintDefaults()
	}
}

const fuchsiaReviewURL = "https://fuchsia-review.googlesource.com"

// fuchsiaRE is a regexp for matching CL review URLs and extracting the CL numbers.
// Supports various forms. E.g.,
//   - https://fuchsia-review.googlesource.com/c/fuchsia/+/123456789
//   - fuchsia-review.googlesource.com/c/fuchsia/+/123456789/some/file
//   - http://fxr/123456789
//   - fxr/123456789/some/file
var fuchsiaRE = regexp.MustCompile(`^(?:https?://)?(?:fxr|fuchsia-review.googlesource.com/c/.+/\+)/(\d+).*`)

func parseReviewURL(str string) (lib.QueryInfo, error) {
	match := fuchsiaRE.FindStringSubmatch(str)
	if match != nil {
		return lib.QueryInfo{
			APIEndpoint: fuchsiaReviewURL,
			CL:          match[1],
		}, nil
	}

	return lib.QueryInfo{}, errors.New("not a valid review URL")
}

func main() {
	flag.Parse()

	if flag.NArg() == 0 {
		// TODO: Consider alternatives. E.g., show all outstanding CLs
		// of the current user, or show all CLs that are pending in
		// Global Integration.
		flag.Usage()
		os.Exit(1)
	}

	queryInfo, err := parseReviewURL(flag.Arg(0))
	if err != nil {
		log.Fatalf("Error parsing the review URL: %v", err)
	}

	ci, err := lib.GetChangeInfo(queryInfo)
	if err != nil {
		log.Fatalf("Error getting change info: %v", err)
	}
	fmt.Printf("CL status: %v\n", ci.Status)

	if ci.Status != lib.CLStatusMerged {
		return
	}

	gs, err := lib.GetGIStatus(ci)
	if err != nil {
		log.Fatalf("Error getting GI status: %v", err)
	}
	fmt.Printf("GI status: %v\n", gs)
}
