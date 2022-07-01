// Copyright 2021 The TrueBlocks Authors. All rights reserved.
// Use of this source code is governed by a license that can
// be found in the LICENSE file.

package chunksPkg

import (
	"github.com/TrueBlocks/trueblocks-core/src/apps/chifra/internal/globals"
	"github.com/TrueBlocks/trueblocks-core/src/apps/chifra/pkg/cache"
	"github.com/TrueBlocks/trueblocks-core/src/apps/chifra/pkg/config"
	"github.com/TrueBlocks/trueblocks-core/src/apps/chifra/pkg/manifest"
	"github.com/TrueBlocks/trueblocks-core/src/apps/chifra/pkg/types"
)

func (opts *ChunksOptions) HandleRepair(blockNums []uint64) error {
	man, err := manifest.ReadManifest(opts.Globals.Chain, manifest.FromCache)
	if err != nil {
		return err
	}

	reports := []types.RepairReport{}

	for _, chunk := range man.Chunks {
		rng, _ := cache.RangeFromFilename(chunk.Range + ".")
		if rng.BlockIntersects(blockNums[0]) {
			report := types.RepairReport{
				Status:    "Repaired Bloom",
				Range:     config.GetPathToIndex(opts.Globals.Chain) + "blooms/" + chunk.Range + ".bloom",
				BloomHash: string(chunk.BloomHash),
				// TODO: BOGUS -- PIN THE FILE TO PINATA -- REPORT THE IPFS HASH
			}
			reports = append(reports, report)
			report = types.RepairReport{
				Status:    "Repaired Index",
				Range:     config.GetPathToIndex(opts.Globals.Chain) + "finalized/" + chunk.Range + ".bin",
				IndexHash: string(chunk.IndexHash),
				// TODO: BOGUS -- PIN THE FILE TO PINATA -- REPORT THE IPFS HASH
			}
			reports = append(reports, report)
		}
	}

	return globals.RenderSlice(&opts.Globals, reports)
}