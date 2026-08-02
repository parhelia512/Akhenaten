log_info("akhenaten: city_monuments.js started")

var MONUMENT_WEIGHTS = {}
MONUMENT_WEIGHTS[BUILDING_PYRAMID]                        = 44
MONUMENT_WEIGHTS[BUILDING_STEPPED_PYRAMID_COMPLEX]        = 24  // on-land (= large); +causeway → recalibrate toward 44
MONUMENT_WEIGHTS[BUILDING_GRAND_STEPPED_PYRAMID_COMPLEX]  = 44
// True complex on-land shares large weight until causeway (W=44 saturates clamp).
MONUMENT_WEIGHTS[BUILDING_PYRAMID_COMPLEX]                = 13
MONUMENT_WEIGHTS[BUILDING_GRAND_PYRAMID_COMPLEX]          = 44
MONUMENT_WEIGHTS[BUILDING_SMALL_MASTABA]                  = 2
MONUMENT_WEIGHTS[BUILDING_MEDIUM_MASTABA]                 = 2
MONUMENT_WEIGHTS[BUILDING_LARGE_MASTABA]                  = 3
// Stepped pyramids: heavier than mastaba (2), lighter than true pyramid complex (44).
MONUMENT_WEIGHTS[BUILDING_SMALL_STEPPED_PYRAMID]           = 8
MONUMENT_WEIGHTS[BUILDING_MEDIUM_STEPPED_PYRAMID]         = 16
MONUMENT_WEIGHTS[BUILDING_LARGE_STEPPED_PYRAMID]          = 24
// Rating is clamped to 100 (city/monuments.js). Weight 44 for stepped complex alone
// already saturates (2.25*44+4.5 > 100), so on-land complex shares large's 24 until
// causeway/temples land or the whole table is recalibrated.
// Bent pyramids (placeholder weights, like the others).
// A finished medium bent gives 2.25*8+4.5 = 22, which satisfies mission 14's
// original monument goal of 21 (Snofru's bent pyramid at South Dahshur).
MONUMENT_WEIGHTS[BUILDING_SMALL_BENT_PYRAMID]             = 4
MONUMENT_WEIGHTS[BUILDING_MEDIUM_BENT_PYRAMID]            = 8
// Mudbrick (brick-core) pyramids — S=4 (not 8) so Itjtawy Sphinx+S+M → rating 33.
MONUMENT_WEIGHTS[BUILDING_SMALL_MUDBRICK_PYRAMID]         = 4
MONUMENT_WEIGHTS[BUILDING_MEDIUM_MUDBRICK_PYRAMID]        = 8
// Large mudbrick 16×16: continue half-stepped series (S/M/L stepped 8/16/24 → 4/8/12).
MONUMENT_WEIGHTS[BUILDING_LARGE_MUDBRICK_PYRAMID]         = 12
// Complex on-land shares large weight (44 would saturate alone); grand keeps 44.
MONUMENT_WEIGHTS[BUILDING_MUDBRICK_PYRAMID_COMPLEX]       = 12
MONUMENT_WEIGHTS[BUILDING_GRAND_MUDBRICK_PYRAMID_COMPLEX] = 44
// True (smooth) pyramids.
MONUMENT_WEIGHTS[BUILDING_SMALL_PYRAMID]                  = 8
MONUMENT_WEIGHTS[BUILDING_MEDIUM_PYRAMID]                 = 16
MONUMENT_WEIGHTS[BUILDING_LARGE_PYRAMID]                  = 13
MONUMENT_WEIGHTS[BUILDING_SPHINX]                         = 1
MONUMENT_WEIGHTS[BUILDING_SMALL_OBELISK]                  = 2
MONUMENT_WEIGHTS[BUILDING_LARGE_OBELISK]                  = 4
MONUMENT_WEIGHTS[BUILDING_SUN_TEMPLE]                     = 4
// OG RawNumber 5 (Heaven chart). trunc(2.25·5+4.5)=15 ≥ Alex₁ goal 14; Heh pak 15.
MONUMENT_WEIGHTS[BUILDING_MAUSOLEUM]                      = 5
// Mission 49 goal 28 needs Σw ≥ 11 (trunc(2.25·Σ+4.5)). Library alone must not
// saturate: W=6 → 18. Pair with Pharos W=5 (PL3) → 29 ≥ 28.
MONUMENT_WEIGHTS[BUILDING_ALEXANDRIA_LIBRARY]             = 6
MONUMENT_WEIGHTS[BUILDING_CAESAREUM]                      = 8  // mission 51: +Mausoleum 5 → rating 33 ≥ goal 24
MONUMENT_WEIGHTS[BUILDING_PHAROS_LIGHTHOUSE]              = 5  // PL3; pair Library 6 → rating 29 ≥ goal 28
MONUMENT_WEIGHTS[BUILDING_SMALL_ROYAL_TOMB]               = 4  // m38 goal 13
MONUMENT_WEIGHTS[BUILDING_ABU_SIMBEL]                     = 44
MONUMENT_WEIGHTS[BUILDING_MEDIUM_ROYAL_TOMB]              = 8  // m39 goal 22
MONUMENT_WEIGHTS[BUILDING_LARGE_ROYAL_TOMB]               = 13 // m40 goal 33 = trunc(2.25*13+4.5)
MONUMENT_WEIGHTS[BUILDING_GRAND_ROYAL_TOMB]               = 18 // m44 goal 45

// Monument rating is APPROXIMATE and additive, not concave. The original Pharaoh
// aggregates monument points roughly linearly across the built monuments; the old
// concave `6.32*sqrt(sum)+0.5` under-counted multiple monuments (3 small mastabas
// gave 15 instead of the original 18). These constants reproduce the known anchor
// points with the current weights:
//   1 small mastaba   (sum 2)  -> 2.25*2 +4.5 = 9   (missions 4/13/16, and On x1)
//   3 small mastabas  (sum 6)  -> 2.25*6 +4.5 = 18  (mission 17 On)
//   1 medium stepped  (sum 16) -> 2.25*16+4.5 = 40  (>= Saqqara verified goal 19)
//   1 small stepped   (sum 8)  -> 2.25*8 +4.5 = 22.5 → trunc 22
//   Meidum S+complex  (8+24)   -> trunc(2.25*32+4.5)=76 (F3a; complex on-land = large)
//   Bent medium       (sum 8)  -> 22 ≥ m14 goal 21
// Exact per-type OG points still need a .pak dump (F3 calibration). Until then
// weights stay placeholders except mission-tuned Cleopatra/Meidum entries above;
// TEMP goals in m_0xx derive from these constants.
var MONUMENT_RATING_MULT = 2.25
var MONUMENT_RATING_OFFSET = 4.5

function monument_weight_btype(list_head_bid) {
	var cur = city.get_building(list_head_bid)
	var guard = 0
	while (cur && guard < 256) {
		guard++
		var t = cur.type
		if (typeof MONUMENT_WEIGHTS[t] !== 'undefined') {
			return t
		}
		if (!cur.next_part_building_id) {
			break
		}
		cur = city.get_building(cur.next_part_building_id)
	}
	return __building_type(list_head_bid)
}

[es=event_advance_month]
function city_update_monthly_monument_rating(ev) {
	var n = __city_monuments_list_refresh()
	if (n == 0) {
		__city_ratings_set_monument(0)
		return
	}

	var sum = 0
	for (var i = 0; i < n; i++) {
		var bid = __city_monuments_list_id_at(i)
		if (!bid) {
			continue
		}
		var bt = monument_weight_btype(bid)
		var w = MONUMENT_WEIGHTS[bt]
		if (!w) {
			continue
		}
		var monument = city.get_monument(bid)
		if (!monument) {
			continue
		}
		var phase = monument.phase()
		var phases = monument.phases_total()
		var mat = monument.material_pct_min()

		var progress = 0
		if (phase == -1) {
			progress = 100
		} else if (phases > 0) {
			progress = ((phase - 1) * 100 + mat) / phases
			if (progress < 0) {
				progress = 0
			}
			if (progress > 100) {
				progress = 100
			}
		}

		sum += (w * progress) / 100
	}

	// Additive: base offset for having any monument at all, plus a linear term.
	// No monument progress -> rating 0 (avoid the offset leaking in on an
	// unbuilt/zero-progress foundation).
	var rating = 0
	if (sum > 0) {
		rating = MONUMENT_RATING_MULT * sum + MONUMENT_RATING_OFFSET
	}
	if (rating < 0) {
		rating = 0
	}
	if (rating > 100) {
		rating = 100
	}
	__city_ratings_set_monument(rating | 0)
}
