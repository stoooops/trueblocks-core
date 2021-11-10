package exec

import (
	"errors"
	"net/http"

	"github.com/TrueBlocks/trueblocks-core/src/apps/chifra/cmd/pins"
	"github.com/TrueBlocks/trueblocks-core/src/apps/chifra/pkg/pinlib"
	"github.com/TrueBlocks/trueblocks-core/src/apps/chifra/pkg/pinlib/manifest"
)

var ErrMultipleModes = errors.New("only one of ?list or ?init is supported")
var ErrNoMode = errors.New("you must choose one of ?list or ?init")

// AdminPins runs pin-operating functions depending on query parameters
func AdminPins(request *http.Request) ([]manifest.PinDescriptor, error) {
	query := request.URL.Query()
	mode := ""
	all := false

	for key := range query {
		if key == "list" ||
			key == "init" ||
			key == "freshen" {
			// Only single mode is supported
			if mode != "" {
				return nil, ErrMultipleModes
			}
			mode = key
		}

		if key == "all" {
			all = true
		}
	}

	// Make sure all required directories exist
	err := pinlib.EstablishDirectories()
	if err != nil {
		return nil, err
	}
	var responseBody []manifest.PinDescriptor
	var responseError error

	switch mode {
	case "list":
		pins, err := pins.List()
		responseBody = pins
		responseError = err
	case "init":
		err := pins.Init(all)
		responseError = err
	default:
		responseError = ErrNoMode
	}

	if responseError != nil {
		return nil, responseError
	}

	return responseBody, nil
}