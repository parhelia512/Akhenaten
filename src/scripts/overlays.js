log_info("akhenaten: overlays started")

var overlays = [
  {
  	id:OVERLAY_RELIGION_OSIRIS,
  	caption:"Osiris",
  	walkers:[FIGURE_PRIEST],
  	buildings:[BUILDING_TEMPLE_OSIRIS, BUILDING_TEMPLE_COMPLEX_OSIRIS, BUILDING_SHRINE_OSIRIS],
  	tooltip_base:0,
  	tooltips:[],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
  	id:OVERLAY_RELIGION_RA,
  	caption:"Ra",
  	walkers:[FIGURE_PRIEST],
  	buildings:[BUILDING_TEMPLE_RA, BUILDING_TEMPLE_COMPLEX_RA, BUILDING_SHRINE_RA],
  	tooltip_base:0,
  	tooltips:[],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_RELIGION_SETH,
    walkers:[FIGURE_PRIEST],
    buildings:[BUILDING_TEMPLE_SETH, BUILDING_TEMPLE_COMPLEX_SETH, BUILDING_SHRINE_SETH],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_NATIVE,
    walkers:[FIGURE_INDIGENOUS_NATIVE, FIGURE_MISSIONARY],
    buildings:[],
    column_type: COLUMN_TYPE_RISK,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_RELIGION_PTAH,
    walkers:[FIGURE_PRIEST],
    buildings:[BUILDING_TEMPLE_PTAH, BUILDING_TEMPLE_COMPLEX_PTAH, BUILDING_SHRINE_PTAH],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_RELIGION_BAST,
    walkers:[FIGURE_PRIEST],
    buildings:[BUILDING_TEMPLE_BAST, BUILDING_TEMPLE_COMPLEX_BAST, BUILDING_SHRINE_BAST],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_WATER,
    walkers:[FIGURE_WATER_CARRIER],
    buildings:[BUILDING_WELL, BUILDING_MENU_BEAUTIFICATION, BUILDING_WATER_LIFT, BUILDING_WATER_SUPPLY],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_APOTHECARY,
    walkers:[FIGURE_HERBALIST],
    buildings:[BUILDING_APOTHECARY],
    column_type: COLUMN_TYPE_POSITIVE,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_RELIGION,
    walkers:[FIGURE_PRIEST],
    buildings:[ BUILDING_FESTIVAL_SQUARE,
                BUILDING_TEMPLE_RA, BUILDING_TEMPLE_COMPLEX_RA, BUILDING_SHRINE_RA,
                BUILDING_TEMPLE_OSIRIS, BUILDING_TEMPLE_COMPLEX_OSIRIS, BUILDING_SHRINE_OSIRIS, ],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_BANDSTAND,
    walkers:[FIGURE_PRIEST],
    buildings:[BUILDING_CONSERVATORY, BUILDING_BANDSTAND],
    column_type: COLUMN_TYPE_POSITIVE,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_BAZAAR_ACCESS,
    walkers:[FIGURE_MARKET_BUYER, FIGURE_MARKET_TRADER],
    buildings:[BUILDING_BAZAAR],
    column_type: COLUMN_TYPE_POSITIVE,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_COUTHOUSE,
    walkers:[FIGURE_MAGISTRATE],
    buildings:[BUILDING_COURTHOUSE],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_CRIME,
    walkers:[FIGURE_CONSTABLE, FIGURE_PROTESTER, FIGURE_ROBBER, FIGURE_TOMB_ROBER],
    buildings:[BUILDING_POLICE_STATION, BUILDING_FESTIVAL_SQUARE],
    column_type: COLUMN_TYPE_RISK,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_PROBLEMS,
    walkers:[],
    buildings:[],
    column_type: COLUMN_TYPE_RISK,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_ROUTING,
    walkers:[],
    buildings:[],
    column_type: COLUMN_TYPE_RISK,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_DAMAGE,
    walkers:[FIGURE_ARCHITECT],
    buildings:[BUILDING_ARCHITECT_POST, BUILDING_FESTIVAL_SQUARE],
    column_type: COLUMN_TYPE_RISK,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_DENTIST,
    walkers:[FIGURE_DENTIST],
    buildings:[BUILDING_DENTIST],
    column_type: COLUMN_TYPE_POSITIVE,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_FERTILITY,
    walkers:[],
    buildings:[],
    column_type: COLUMN_TYPE_POSITIVE,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_LIBRARY,
    walkers:[FIGURE_LIBRARIAN],
    buildings:[BUILDING_LIBRARY],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_ACADEMY,
    walkers:[FIGURE_LIBRARIAN],
    buildings:[BUILDING_LIBRARY],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_EDUCATION,
    walkers:[FIGURE_SCRIBER, FIGURE_LIBRARIAN, FIGURE_TEACHER],
    buildings:[BUILDING_SCRIBAL_SCHOOL, BUILDING_LIBRARY, BUILDING_ACADEMY],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_ENTERTAINMENT,
    walkers:[FIGURE_JUGGLER, FIGURE_MUSICIAN, FIGURE_DANCER, FIGURE_SENET_PLAYER, FIGURE_CHARIOR_RACER],
    buildings:[ BUILDING_JUGGLER_SCHOOL, BUILDING_BOOTH, BUILDING_CONSERVATORY,
                BUILDING_BANDSTAND, BUILDING_DANCE_SCHOOL, BUILDING_PAVILLION,
                BUILDING_SENET_MASTER, BUILDING_SENET_HOUSE],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_SENET_HOUSE,
    walkers:[FIGURE_CHARIOR_RACER],
    buildings:[BUILDING_SENET_MASTER, BUILDING_SENET_HOUSE],
    column_type:COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_FIRE,
    walkers:[FIGURE_FIREMAN],
    buildings:[BUILDING_FIREHOUSE, BUILDING_BURNING_RUIN, BUILDING_FESTIVAL_SQUARE],
    column_type: COLUMN_TYPE_RISK,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_HEALTH,
    walkers:[FIGURE_EMBALMER, FIGURE_HERBALIST, FIGURE_PHYSICIAN, FIGURE_DENTIST],
    buildings:[BUILDING_MORTUARY, BUILDING_APOTHECARY, BUILDING_PHYSICIAN, BUILDING_DENTIST],
    column_type: COLUMN_TYPE_RISK,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_LABOR,
    walkers:[],
    buildings:[],
    column_type: COLUMN_TYPE_RISK,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_BOOTH,
    walkers:[FIGURE_JUGGLER],
    buildings:[BUILDING_JUGGLER_SCHOOL, BUILDING_BOOTH],
    column_type: COLUMN_TYPE_POSITIVE,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_MORTUARY,
    walkers:[FIGURE_EMBALMER],
    buildings:[BUILDING_MORTUARY],
    column_type: COLUMN_TYPE_POSITIVE,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_FOOD_STOCKS,
    walkers:[FIGURE_EMBALMER],
    buildings:[BUILDING_BAZAAR, BUILDING_FISHING_WHARF, BUILDING_GRANARY],
    column_type: COLUMN_TYPE_RISK,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_PAVILION,
    walkers:[],
    buildings:[BUILDING_CONSERVATORY, BUILDING_DANCE_SCHOOL, BUILDING_PAVILLION],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_PHYSICIAN,
    walkers:[FIGURE_PHYSICIAN],
    buildings:[BUILDING_PHYSICIAN],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_SCRIBAL_SCHOOL,
    walkers:[FIGURE_TEACHER],
    buildings:[BUILDING_SCRIBAL_SCHOOL],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_DESIRABILITY,
    walkers:[],
    buildings:[],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },

  {
    id:OVERLAY_TAX_INCOME,
    walkers:[FIGURE_TAX_COLLECTOR],
    buildings:[BUILDING_TAX_COLLECTOR, BUILDING_TAX_COLLECTOR_UPGRADED, BUILDING_VILLAGE_PALACE, BUILDING_TOWN_PALACE, BUILDING_CITY_PALACE],
    column_type: COLUMN_TYPE_WATER_ACCESS,
    column_anim: {pack:PACK_GENERAL, id:103},
  },
]
