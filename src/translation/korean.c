#include "translation/common.h"
#include "translation/translation.h"

static translation_string all_strings[] = {
    {TR_NO_PATCH_TITLE, "1.0.1.0 패치가 설치되지 않음"},
    {TR_NO_PATCH_MESSAGE,
        "현재 설치된 시저 3는 1.0.1.0 패치를 포함하고 있지 않습니다. "
        "패치는 이곳에서 다운로드하실 수 있습니다. \n"
        URL_PATCHES "\n"
        "패치 없이 진행하는 데 따른 문제는 본인 책임입니다."},
    // Do not translate the following message since it cannot be shown in Korean:
    {TR_MISSING_FONTS_TITLE, "Missing fonts"},
    {TR_MISSING_FONTS_MESSAGE,
        "Your Caesar 3 installation requires extra font files. "
        "You can download them for your language from:\n"
        URL_PATCHES},
    {TR_NO_EDITOR_TITLE, "편집기가 설치되지 않음"},
    {TR_NO_EDITOR_MESSAGE,
        "현재 설치된 시저 3는 편집기 파일을 포함하고 있지 않습니다. "
        "편집기는 이곳에서 다운로드하실 수 있습니다. \n"
        URL_EDITOR},
    {TR_INVALID_LANGUAGE_TITLE, "잘못된 언어 디렉터리"},
    {TR_INVALID_LANGUAGE_MESSAGE,
        "현재 선택된 폴더는 사용 가능한 언어 팩을 포함하고 있지 않습니다. "
        "기록에서 오류를 확인해 주세요."},
    {TR_BUILD_ALL_TEMPLES, "모두"},
    {TR_BUTTON_OK, "확인"},
    {TR_BUTTON_CANCEL, "취소"},
    {TR_BUTTON_RESET_DEFAULTS, "기본값으로 초기화"},
    {TR_BUTTON_CONFIGURE_HOTKEYS, "단축키 설정"},
    {TR_CONFIG_TITLE, "Augustus 설정 옵션"},
    {TR_CONFIG_LANGUAGE_LABEL, "언어:"},
    {TR_CONFIG_LANGUAGE_DEFAULT, "(기본)"},
    {TR_CONFIG_DISPLAY_SCALE, "화면 배율:"},
    {TR_CONFIG_CURSOR_SCALE, "커서 배율:"},
    {TR_CONFIG_HEADER_UI_CHANGES, "사용자 인터페이스 변경"},
    {TR_CONFIG_HEADER_GAMEPLAY_CHANGES, "게임플레이 변경"},
    {TR_CONFIG_SHOW_INTRO_VIDEO, "시작 영상 재생"},
    {TR_CONFIG_SIDEBAR_INFO, "게임 제어판에 추가 정보 표시"},
    {TR_CONFIG_SMOOTH_SCROLLING, "부드러운 화면이동 가능"},
    {TR_CONFIG_DISABLE_MOUSE_EDGE_SCROLLING, "창 가장자리 화면이동 끄기"},
    {TR_CONFIG_DISABLE_RIGHT_CLICK_MAP_DRAG, "오른 클릭 드래그 화면이동 끄기"},
    {TR_CONFIG_VISUAL_FEEDBACK_ON_DELETE, "지형 평탄화 시 시각적 표현 향상"},
    {TR_CONFIG_ALLOW_CYCLING_TEMPLES, "각각의 신전을 연속으로 설치 가능"},
    {TR_CONFIG_SHOW_WATER_STRUCTURE_RANGE, "저수조, 분수와 우물 설치 시 범위 표시"},
    {TR_CONFIG_SHOW_CONSTRUCTION_SIZE, "드래그 건설 시 전체 크기 표시"},
    {TR_CONFIG_HIGHLIGHT_LEGIONS, "커서가 올라간 군단 강조"},
    {TR_CONFIG_SHOW_MILITARY_SIDEBAR, "군단 제어판 표시"},
    {TR_CONFIG_ROTATE_MANUALLY, "단축키로 성문 및 개선문 회전"},
    {TR_CONFIG_DISABLE_RIGHT_CLICK_MAP_DRAG, "오른쪽 마우스 버튼으로 드래그 불가"},
    {TR_CONFIG_FIX_IMMIGRATION_BUG, "매우 어려움 난이도 이주 문제 수정"},
    {TR_CONFIG_FIX_100_YEAR_GHOSTS, "100세 이상 고령 주민 문제 수정"},
    {TR_CONFIG_FIX_EDITOR_EVENTS, "커스텀 임무에서 황제 교체와 생존 시간 문제 수정"},
    {TR_CONFIG_DRAW_WALKER_WAYPOINTS, "건물 오른 클릭 후 오버레이에 보행자 경로 그리기"},
    {TR_CONFIG_GRANDFESTIVAL, "대축제를 개최하여 해당 신들의 추가 축복 "},
    {TR_CONFIG_JEALOUS_GODS, "신들의 질투 비활성화"},
    {TR_CONFIG_GLOBAL_LABOUR, "전역적인 인력 활용 활성화"},
    {TR_CONFIG_SCHOOL_WALKERS, "학교 강사 범위 확장"},
    {TR_CONFIG_RETIRE_AT_60, "시민의 은퇴연령을 50세에서 60로 연장"},
    {TR_CONFIG_FIXED_WORKERS, "고정된 작업자 모임 - 평민 인구의 38%"},
    {TR_CONFIG_EXTRA_FORTS, "요새 4채 추가 건설 허용"},
    {TR_CONFIG_WOLVES_BLOCK, "늑대를 둘러싼 건설 막기"},
    {TR_CONFIG_DYNAMIC_GRANARIES, "연결되지 않은 곡물창고의 도로 차단"},
    {TR_CONFIG_MORE_STOCKPILE, "주택의 상품 비축량 증가"},
    {TR_CONFIG_NO_SUPPLIER_DISTRIBUTION, "시장의 도매상이 상품을 분배하지 않음"},
    {TR_CONFIG_IMMEDIATELY_DELETE_BUILDINGS, "건물 즉시 파괴"},
    {TR_CONFIG_GETTING_GRANARIES_GO_OFFROAD, "다른 창고에서 상품을 입수하는 수레꾼은 길이 없더라도 이동 가능"},
    {TR_CONFIG_GRANARIES_GET_DOUBLE, "식량을 입수하는 곡물 창고의 수레꾼의 효율이 두 배로 증가"},
    {TR_CONFIG_TOWER_SENTRIES_GO_OFFROAD, "병영에서 탑으로 접근할 수 있는 도로가 필요없음"},
    {TR_CONFIG_FARMS_DELIVER_CLOSE, "농장과 선창은 근처 곡물창고로 배달"},
    {TR_CONFIG_DELIVER_ONLY_TO_ACCEPTING_GRANARIES, "입수 중인 곡물창고로 음식을 배달하지 않음"},
    {TR_CONFIG_ALL_HOUSES_MERGE, "모든 주택 병합"},
    {TR_CONFIG_WINE_COUNTS_IF_OPEN_TRADE_ROUTE, "개통된 무역로의 와인은 서로 다른 타입으로 간주"},
    {TR_CONFIG_RANDOM_COLLAPSES_TAKE_MONEY, "점토채굴장의 침수피해 및 광산 붕괴 대신 자금 소모"},
    {TR_CONFIG_MULTIPLE_BARRACKS, "여러 병영 건설 허용" },
    {TR_CONFIG_NOT_ACCEPTING_WAREHOUSES, "저장소의 물품들이 보관 불가로 초기 설정됨"},
    {TR_CONFIG_HOUSES_DONT_EXPAND_INTO_GARDENS, "정원을 넘어선 주택 확장 불가"},
    {TR_HOTKEY_TITLE, "Augustus 단축키 설정"},
    {TR_HOTKEY_LABEL, "단축키"},
    {TR_HOTKEY_ALTERNATIVE_LABEL, "대체"},
    {TR_HOTKEY_HEADER_ARROWS, "화살표키"},
    {TR_HOTKEY_HEADER_GLOBAL, "전역 단축키"},
    {TR_HOTKEY_HEADER_CITY, "도시 단축키"},
    {TR_HOTKEY_HEADER_ADVISORS, "자문관"},
    {TR_HOTKEY_HEADER_OVERLAYS, "맵보기"},
    {TR_HOTKEY_HEADER_BOOKMARKS, "도시 맵 즉시 이동"},
    {TR_HOTKEY_HEADER_EDITOR, "편집기"},
    {TR_HOTKEY_HEADER_BUILD, "건설 단축키"},
    {TR_HOTKEY_ARROW_UP, "위"},
    {TR_HOTKEY_ARROW_DOWN, "아래"},
    {TR_HOTKEY_ARROW_LEFT, "왼쪽"},
    {TR_HOTKEY_ARROW_RIGHT, "오른쪽"},
    {TR_HOTKEY_TOGGLE_FULLSCREEN, "전체화면 전환"},
    {TR_HOTKEY_CENTER_WINDOW, "창을 화면 가운데로"},
    {TR_HOTKEY_RESIZE_TO_640, "창 크기 640x480으로 변경"},
    {TR_HOTKEY_RESIZE_TO_800, "창 크기 800x600으로 변경"},
    {TR_HOTKEY_RESIZE_TO_1024, "창 크기 1024x768로 변경"},
    {TR_HOTKEY_SAVE_SCREENSHOT, "스크린샷 저장"},
    {TR_HOTKEY_SAVE_CITY_SCREENSHOT, "도시 전체 스크린샷 저장"},
    {TR_HOTKEY_BUILD_CLONE, "커서 아래에 있는 건물 복제"},
    {TR_HOTKEY_LOAD_FILE, "파일 불러오기"},
    {TR_HOTKEY_SAVE_FILE, "파일 저장하기"},
    {TR_HOTKEY_INCREASE_GAME_SPEED, "게임 속도 증가"},
    {TR_HOTKEY_DECREASE_GAME_SPEED, "게임 속도 감소"},
    {TR_HOTKEY_TOGGLE_PAUSE, "일시 정지 전환"},
    {TR_HOTKEY_CYCLE_LEGION, "군단 돌아가며 보기"},
    {TR_HOTKEY_ROTATE_MAP_LEFT, "시계 반대 방향으로 맵 회전"},
    {TR_HOTKEY_ROTATE_MAP_RIGHT, "시계 방향으로 맵 회전"},
    {TR_HOTKEY_ROTATE_BUILDING, "건물 회전"},
    {TR_HOTKEY_SHOW_ADVISOR_LABOR, "노동 자문관"},
    {TR_HOTKEY_SHOW_ADVISOR_MILITARY, "군단 자문관"},
    {TR_HOTKEY_SHOW_ADVISOR_IMPERIAL, "황제 자문관"},
    {TR_HOTKEY_SHOW_ADVISOR_RATINGS, "등급 자문관"},
    {TR_HOTKEY_SHOW_ADVISOR_TRADE, "무역 자문관"},
    {TR_HOTKEY_SHOW_ADVISOR_POPULATION, "인구 자문관"},
    {TR_HOTKEY_SHOW_ADVISOR_HEALTH, "보건 자문관"},
    {TR_HOTKEY_SHOW_ADVISOR_EDUCATION, "교육 자문관"},
    {TR_HOTKEY_SHOW_ADVISOR_ENTERTAINMENT, "오락 자문관"},
    {TR_HOTKEY_SHOW_ADVISOR_RELIGION, "종교 자문관"},
    {TR_HOTKEY_SHOW_ADVISOR_FINANCIAL, "재정 자문관"},
    {TR_HOTKEY_SHOW_ADVISOR_CHIEF, "수석 자문관"},
    {TR_HOTKEY_SHOW_ADVISOR_HOUSING, "주택 자문관"},
    {TR_HOTKEY_TOGGLE_OVERLAY, "현재 맵보기 전환"},
    {TR_HOTKEY_SHOW_OVERLAY_WATER, "물공급 맵보기 전환"},
    {TR_HOTKEY_SHOW_OVERLAY_FIRE, "화재 맵보기 전환"},
    {TR_HOTKEY_SHOW_OVERLAY_DAMAGE, "손상 맵보기 전환"},
    {TR_HOTKEY_SHOW_OVERLAY_CRIME, "범죄 맵보기 전환"},
    {TR_HOTKEY_SHOW_OVERLAY_PROBLEMS, "문제 맵보기 전환"},
    {TR_HOTKEY_GO_TO_BOOKMARK_1, "1지점으로 이동"},
    {TR_HOTKEY_GO_TO_BOOKMARK_2, "2지점으로 이동"},
    {TR_HOTKEY_GO_TO_BOOKMARK_3, "3지점으로 이동"},
    {TR_HOTKEY_GO_TO_BOOKMARK_4, "4지점으로 이동"},
    {TR_HOTKEY_SET_BOOKMARK_1, "1지점 설정"},
    {TR_HOTKEY_SET_BOOKMARK_2, "2지점 설정"},
    {TR_HOTKEY_SET_BOOKMARK_3, "3지점 설정"},
    {TR_HOTKEY_SET_BOOKMARK_4, "4지점 설정"},
    {TR_HOTKEY_EDITOR_TOGGLE_BATTLE_INFO, "전투 정보 전환"},
    {TR_HOTKEY_EDIT_TITLE, "새 단축키 입력"},
    {TR_BUILDING_ROADBLOCK, "도로 블록"},
    {TR_BUILDING_ROADBLOCK_DESC, "도로 블록은 배회하는 시민들을 가로막습니다."},
    {TR_BUILDING_ARCHITECT_GUILD, "기술자 길드" },
    {TR_BUILDING_ARCHITECT_GUILD_DESC, "이곳의 기술자들은 로마의 영광을 위한 기념물을 세우기 위해 부지런히 일합니다." },
    {TR_BUILDING_GRAND_TEMPLE_ADD_MODULE, "신전에 속성을 부여한다" },
    {TR_BUILDING_GRAND_TEMPLE_MENU, "초대형 신전" },
    {TR_BUILDING_GRAND_TEMPLE_CERES, "케레스 대성전" },
    {TR_BUILDING_GRAND_TEMPLE_NEPTUNE, "넵튠 대성전" },
    {TR_BUILDING_GRAND_TEMPLE_MERCURY, "머큐리 대성전" },
    {TR_BUILDING_GRAND_TEMPLE_MARS, "마르스 대성전" },
    {TR_BUILDING_GRAND_TEMPLE_VENUS, "비너스 대성전" },
    {TR_BUILDING_PANTHEON, "판테온" },
    {TR_BUILDING_GRAND_TEMPLE_CERES_DESC, "케레스의 대성전" },
    {TR_BUILDING_GRAND_TEMPLE_NEPTUNE_DESC, "넵튠의 대성전" },
    {TR_BUILDING_GRAND_TEMPLE_MERCURY_DESC, "머큐리의 대성전" },
    {TR_BUILDING_GRAND_TEMPLE_MARS_DESC, "마르스의 대성전" },
    {TR_BUILDING_GRAND_TEMPLE_VENUS_DESC, "비너스의 대성전" },
    {TR_BUILDING_PANTHEON_DESC, "판테온" },
    {TR_BUILDING_GRAND_TEMPLE_CERES_BONUS_DESC, "농장에서 나오는 짐수레꾼이 50% 더 빠르게 움직입니다." },
    {TR_BUILDING_GRAND_TEMPLE_NEPTUNE_BONUS_DESC, "우물 및 분수의 효과 범위가 +1, 저수조의 범위가 +2 증가합니다. 물공급에 드는 노동력이 50% 감소합니다. 무역선이 25% 빠르게 이동합니다." },
    {TR_BUILDING_GRAND_TEMPLE_MERCURY_BONUS_DESC, "육로 및 해상 무역이 50%만큼 더 많은 물품을 교역합니다. 육로 무역상이 25% 더 빠르게 움직입니다." },
    {TR_BUILDING_GRAND_TEMPLE_MARS_BONUS_DESC, "대성전이 그 자체로 병영처럼 병사를 훈련시키며, 요새 4개를 더 건설할 수 있습니다." },
    {TR_BUILDING_GRAND_TEMPLE_VENUS_BONUS_DESC, "도시의 정원, 석상, 신전 등이 더욱 아름답게 빛나며, 주거환경을 더욱 높여줍니다. 집들이 더 많은 상품을 비축할 수 있으며 좀 더 늦게 퇴보합니다." },
    {TR_BUILDING_PANTHEON_BONUS_DESC, "모든 종류의 신에 대한 숭배를 제공합니다. 매년마다 축제를 열어 주고 신전 유지비를 25% 감소시킵니다." },
    {TR_BUILDING_GRAND_TEMPLE_CERES_MODULE_1_DESC, "성직자들이 식량 소모량을 20% 감소시킵니다." },
    {TR_BUILDING_GRAND_TEMPLE_NEPTUNE_MODULE_1_DESC, "신전들이 전차경기장에 쓰이는 전차경주자를 양성합니다." },
    {TR_BUILDING_GRAND_TEMPLE_MERCURY_MODULE_1_DESC, "성직자들이 도기와 가구 소모량을 20%만큼 감소시킵니다." },
    {TR_BUILDING_GRAND_TEMPLE_MARS_MODULE_1_DESC, "마르스 신전들이 해당 신전 성직자들의 서비스를 받는 집들 갯수에 비례한 만큼의 식량을 자체 생산해 비축한 뒤, 그것들을 도시의 보급창에 배달합니다." },
    {TR_BUILDING_GRAND_TEMPLE_VENUS_MODULE_1_DESC, "대성전이 비너스 신전의 서비스를 받는 도시 인구에 비례한 만큼의 포도주를 생산합니다. 각 비너스 신전들은 이것을 수집한 다음 집들에 분배합니다." },
    {TR_BUILDING_GRAND_TEMPLE_CERES_MODULE_2_DESC, "성직자들이 기름과 식량 1가지를 도시 저장고에서 수집해 분배합니다." },
    {TR_BUILDING_GRAND_TEMPLE_NEPTUNE_MODULE_2_DESC, "성직자들이 주택의 인구 수용량을 5%만큼 증가시킵니다. 대성전이 꽉 찬 저수조와 같은 기능을 하며 주변에 물을 공급합니다." },
    {TR_BUILDING_GRAND_TEMPLE_MERCURY_MODULE_2_DESC, "성직자들이 기름과 포도주 소모량을 20%만큼 감소시킵니다." },
    {TR_BUILDING_GRAND_TEMPLE_MARS_MODULE_2_DESC, "성직자들이 모든 물품의 소모량을 10%만큼 감소시킵니다." },
    {TR_BUILDING_GRAND_TEMPLE_VENUS_MODULE_2_DESC, "성직자들이 자체적으로 오락을 제공하며 주택의 미관을 개선하고 돌아다녀서, 별개의 장식물 없이도 거리의 아름다움을 올려줍니다. " },
    {TR_BUILDING_PANTHEON_MODULE_1_DESC, "모든 도시의 신전들이 각자 성직자를 판테온으로 보내서 서로의 신앙과 축복을 퍼뜨립니다." },
    {TR_BUILDING_PANTHEON_MODULE_2_DESC, "판테온의 영향을 받은 주택은 1단계 위로 진화합니다 ." },
    {TR_BUILDING_GRAND_TEMPLE_CERES_DESC_MODULE_1, "케레스 프로미토르의 신전" },
    {TR_BUILDING_GRAND_TEMPLE_CERES_DESC_MODULE_2, "케레스 레파라토르의 신전" },
    {TR_BUILDING_GRAND_TEMPLE_NEPTUNE_DESC_MODULE_1, "넵투누스 에퀘스테르의 신전" },
    {TR_BUILDING_GRAND_TEMPLE_NEPTUNE_DESC_MODULE_2, "넵투누스 아디우토르의 신전" },
    {TR_BUILDING_GRAND_TEMPLE_MERCURY_DESC_MODULE_1, "메르쿠리우스 포르투누스의 신전" },
    {TR_BUILDING_GRAND_TEMPLE_MERCURY_DESC_MODULE_2, "메르쿠리우스 아분단티아의 신전" },
    {TR_BUILDING_GRAND_TEMPLE_MARS_DESC_MODULE_1, "마르스 울토르의 신전" },
    {TR_BUILDING_GRAND_TEMPLE_MARS_DESC_MODULE_2, "마르스 퀴리누스의 신전" },
    {TR_BUILDING_GRAND_TEMPLE_VENUS_DESC_MODULE_1, "베누스 베르티코르디아의 신전" },
    {TR_BUILDING_GRAND_TEMPLE_VENUS_DESC_MODULE_2, "베누스 게네트릭스의 신전" },
    {TR_BUILDING_PANTHEON_DESC_MODULE_1, "판테움 아라 막시마" },
    {TR_BUILDING_PANTHEON_DESC_MODULE_2, "판테움 로마 아에테르나" },
    {TR_BUILDING_GRAND_TEMPLE_MENU, "대성전" },
    {TR_BUILDING_WORK_CAMP, "노동자 숙소" },
    {TR_BUILDING_WORK_CAMP_DESC, "건설현장에 자재를 나르는 노동자들이 여기 모입니다." },
    {TR_HEADER_HOUSING, "주택"},
    {TR_ADVISOR_HOUSING_ROOM, "주택을 신축할 만한 공간이 있습니다."},
    {TR_ADVISOR_HOUSING_NO_ROOM, "신축 주택을 위한 공간이 없습니다."},
    {TR_ADVISOR_RESIDENCES_DEMANDING_POTTERY, "입주자들이 도기를 필요로 함"},
    {TR_ADVISOR_RESIDENCES_DEMANDING_FURNITURE, "입주자들이 가구를 필요로 함"},
    {TR_ADVISOR_RESIDENCES_DEMANDING_OIL, "입주자들이 기름을 필요로 함"},
    {TR_ADVISOR_RESIDENCES_DEMANDING_WINE, "입주자들이 포도주를 필요로 함"},
    {TR_ADVISOR_TOTAL_NUM_HOUSES, "총 입주자:"},
    {TR_ADVISOR_AVAILABLE_HOUSING_CAPACITY, "입주 가능 인원:"},
    {TR_ADVISOR_TOTAL_HOUSING_CAPACITY, "총 입주 가능 인원:"},
    {TR_ADVISOR_ADVISOR_HEADER_HOUSING, "인구 - 주택"},
    {TR_ADVISOR_BUTTON_GRAPHS, "통계"},
    {TR_ADVISOR_HOUSING_PROSPERITY_RATING, "주택 번영도는"},
    {TR_ADVISOR_PERCENTAGE_IN_VILLAS_PALACES, "장원과 궁전에 거주하는 인구의 비율은"},
    {TR_ADVISOR_PERCENTAGE_IN_TENTS_SHACKS, "텐트와 통나무집에 거주하는 인구의 비율은"},
    {TR_ADVISOR_AVERAGE_TAX, "가구당 평균 세입은"},
    {TR_ADVISOR_AVERAGE_AGE, "가구당 평균 연령은"},
    {TR_ADVISOR_PERCENT_IN_WORKFORCE, "인구 중 노동자가 차지하는 비중은"},
    {TR_ADVISOR_BIRTHS_LAST_YEAR, "작년의 출생자 수:"},
    {TR_ADVISOR_DEATHS_LAST_YEAR, "작년의 사망자 수:"},
    {TR_ADVISOR_TOTAL_POPULATION, "총 가구 수"},
    {TR_REQUIRED_RESOURCES, "현 단계에 배달된 자원:"},
    {TR_BUILDING_GRAND_TEMPLE_CONSTRUCTION_DESC, "대성전을 건축하는 데는 창고에 비축된 원자재, 노동자 숙소의 노동자들, 기술자 길드의 기술자들이 필요합니다."},
    {TR_CONSTRUCTION_PHASE, "건축 단계:"},
    {TR_ADD_MODULE, "신전 재봉헌?"},
    {TR_BUILDING_TEMPLE_MODULE_CONSTRUCTED, "속성 부여됨."},
    {TR_BUILDING_CERES_TEMPLE_QUOTE, "태초에 케레스가 서투른 등짝에 쟁기질 요령을 가르치고\n수태하는 대지에 태동하는 씨앗을 뿌리라고 내려주셨네.\n그녀는 인간에게 처음으로 풍족한 먹거리를 베푸시고\n이 못된 세상에서 바른 법을 정하셨네. \n - 오비디우스, 변신 이야기, 5권"},
    { TR_BUILDING_NEPTUNE_TEMPLE_QUOTE, "넵투누스가 망치를 내려놓으니 구름이 떨어지며\n바다도 그 골이 패인 얼굴을 바로 펴는구나.\n트리톤이 그 부름을 받들어 파도 위로 모습을 드러내는데;\n진홍빛 망토가 나부끼고;\n손에는 배배 꼬인 나발을 비껴 들었더라.\n-오비디우스, 변신 이야기, 1권"},
    {TR_BUILDING_MERCURY_TEMPLE_QUOTE, "소와 살진 양들은 노략질하면 되고,\n세 발 솥단지나 밤색 준마는 흥정을 해봄직도 하구나.\n그러나 끊어진 사람 숨은 다시 돌릴 수가 없으리라-\n어떤 노략꾼이 힘으로 을러도, 어떤 거래를 한다 해도, \n일단 사람의 악문 잇새로 그것이 새나간 뒤에는.\n-호메로스, 일리아드, 9권"},
    {TR_BUILDING_MARS_TEMPLE_QUOTE, "마르스를 위하여, 그의 전차축을 고치고 기운을 다시 다져 \n벼려진 칼날을 쥐여 다시 한번 내보내,\n나팔의 우렁찬 소리로 지지부진한 전쟁을 뒤흔들게 하여라.\n-베르길리우스, 아이네이스, 8권"},
    {TR_BUILDING_VENUS_TEMPLE_QUOTE, "날개 돋은 큐피드와 그의 살가운 어머니 베누스 덕에,\n내 얼마나 큰 기쁨을 누렸던가!\n얼마나 분명히 열정이 나를 휘감았던가!\n내 가슴이 내가 누운 자리를 어찌나 뜨겁게 녹였던가!\n-오비디우스, 변신 이야기, 4권"},
    {TR_BUILDING_PANTHEON_QUOTE, "천상의 문이 열리고 유피테르가 모두를 불러 모으니\n신들이 회당에 모여 담화 나눈다.\n거룩히 앉아 그가 멀찍이서 모든 것을 둘러본다. \n들판, 야영지, 전쟁의 향방, \n그리고 모든 하계의 삼라만상을 처음부터 끝까지. \n지고한 회의가 여기 열렸다.\n-베르길리우스, 아이네이스, 10권"},
    {TR_BUILDING_GRAND_TEMPLE_PHASE_1, "(포디움)"},
    {TR_BUILDING_GRAND_TEMPLE_PHASE_2, "(포르티코)"},
    {TR_BUILDING_GRAND_TEMPLE_PHASE_3, "(켈라)"},
    {TR_BUILDING_GRAND_TEMPLE_PHASE_4, "(프라이킹크툼)"},
    {TR_BUILDING_GRAND_TEMPLE_PHASE_5, "(데디카티오)"},
    {TR_BUILDING_GRAND_TEMPLE_PHASE_1_TEXT, "노동자들이 신전의 높이를 확보하고 세월이 흘러도 버티게 해 줄 포디움을 짓는 중입니다." },
    {TR_BUILDING_GRAND_TEMPLE_PHASE_2_TEXT, "장인들이 신전의 제단을 받치는 포르티코를 만드는 중입니다." },
    {TR_BUILDING_GRAND_TEMPLE_PHASE_3_TEXT, "기술자들이 신성한 초상들이 새겨진 신전의 내부 인테리어인 켈라를 만드는 중입니다." },
    {TR_BUILDING_GRAND_TEMPLE_PHASE_4_TEXT, "숭배자들이 야외에 모여 공공 행사를 하는 장소인 신전 프라이킹크툼의 공사가 시작되었습니다." },
    {TR_BUILDING_GRAND_TEMPLE_PHASE_5_TEXT, "신전 공사가 거의 끝났으므로 성직자들이 모여 축복 의식을 하고 있습니다." },
    {TR_BUILDING_MENU_TREES, "가로수" },
    {TR_BUILDING_MENU_PATHS, "도로" },
    {TR_BUILDING_MENU_PARKS, "공원"},
    {TR_BUILDING_SMALL_POND, "소형 연못"},
    {TR_BUILDING_LARGE_POND, "대형 연못"},
    {TR_BUILDING_PINE_TREE, "소나무"},
    {TR_BUILDING_FIR_TREE, "전나무" },
    {TR_BUILDING_OAK_TREE, "참나무" },
    {TR_BUILDING_ELM_TREE, "느릅나무" },
    {TR_BUILDING_FIG_TREE, "무화과나무" },
    {TR_BUILDING_PLUM_TREE, "자두나무" },
    {TR_BUILDING_PALM_TREE, "야자수" },
    {TR_BUILDING_DATE_TREE, "대추야자나무" },
    {TR_BUILDING_PINE_PATH, "소나무길" },
    {TR_BUILDING_FIR_PATH, "전나무길" },
    {TR_BUILDING_OAK_PATH, "참나무길" },
    {TR_BUILDING_ELM_PATH, "느릅나무길" },
    {TR_BUILDING_FIG_PATH, "무화과나무길" },
    {TR_BUILDING_PLUM_PATH, "자두나무길" },
    {TR_BUILDING_PALM_PATH, "야자수길" },
    {TR_BUILDING_DATE_PATH, "대추야자나무길" },
    {TR_BUILDING_BLUE_PAVILION, "청색 정자" },
    {TR_BUILDING_RED_PAVILION, "적색 정자"},
    {TR_BUILDING_ORANGE_PAVILION, "주황색 정자"},
    {TR_BUILDING_YELLOW_PAVILION, "노랑색 정자"},
    {TR_BUILDING_GREEN_PAVILION, "초록색 정자" },
    {TR_BUILDING_SMALL_STATUE_ALT, "여신 석상" },
    {TR_BUILDING_SMALL_STATUE_ALT_B, "원로원 의원상"},
    {TR_BUILDING_OBELISK, "오벨리스크" },
    {TR_BUILDING_POND_DESC, "저수조에서 끌어온 물로 채워진 연못은 동식물에게 물을 제공하고 사람들에게는 시원하고 느긋한 오아시스가 되어줍니다. 모든 시민들은 연못 근처에서 살고 싶어합니다." },
    {TR_BUILDING_WINDOW_POND, "연못"},
    {TR_BUILDING_OBELISK_DESC, "나일 강의 고대 왕이 만든 고귀한 기념물입니다. 이집트인들이 버려둔 것입니다."},
    {TR_ADVISOR_FINANCE_LEVIES, "건물 유지비"},
    {TR_CONFIRM_DELETE_MONUMENT, "이 기념물 파괴"},
    {TR_SELECT_EPITHET_PROMPT_HEADER, "부여할 속성 선택"},
    {TR_SELECT_EPITHET_PROMPT_TEXT, "속성을 부여하는 것은 이 성전의 신이 가진 어떤 한 가지 모습에 대해 초점을 맞춰 영원히 신전을 봉헌하고 성직자들에게 특정한 종류의 신통력을 부여하는 행위입니다. 이것은 1000자금을 소모합니다." },
    {TR_BUILDING_INFO_MONTHLY_LEVY, "/달"},
    {TR_BUILDING_MESS_HALL, "보급창"},
    {TR_BUILDING_MESS_HALL_DESC, "보급창은 도시의 곡물창고에서 식량을 거둬들여 도시의 요새에 주둔하는 병사들을 먹입니다. 보급이 제대로 되지 않으면 징병이 느려지고 병사들의 사기가 추락합니다."},
    {TR_BUILDING_MESS_HALL_FULFILLMENT, "지난 달의 식량 보급상태."},
    {TR_BUILDING_MESS_HALL_TROOP_HUNGER, "병사들은: " },
    {TR_BUILDING_MESS_HALL_TROOP_HUNGER_1, "식량이 풍족함" },
    {TR_BUILDING_MESS_HALL_TROOP_HUNGER_2, "식량이 충분함"},
    {TR_BUILDING_MESS_HALL_TROOP_HUNGER_3, "배고픔"},
    {TR_BUILDING_MESS_HALL_TROOP_HUNGER_4, "아주 배고픔"},
    {TR_BUILDING_MESS_HALL_TROOP_HUNGER_5, "굶주림에 시달림"},
    {TR_BUILDING_MESS_HALL_FOOD_TYPES_BONUS_1, "다양한 종류의 식단은 크게 사기를 증진시킵니다."},
    {TR_BUILDING_MESS_HALL_FOOD_TYPES_BONUS_2, "종류가 다양하고 영양이 풍부한 식단을 공급하면 병사들의 사기는 엄청나게 증가됩니다."},
    {TR_BUILDING_MESS_HALL_NO_SOLDIERS, "당신은 식량을 공급할 병사가 없습니다." },
    {TR_BUILDING_MESS_HALL_MONTHS_FOOD_STORED, "다음 개월분의 식량이 남아있습니다:" },
    {TR_BUILDING_BARRACKS_FOOD_WARNING, "보급창의 식량 부족 때문에 병사 모집이 느려지고 있습니다."},
    {TR_BUILDING_BARRACKS_FOOD_WARNING_2, "보급창의 심각한 식량 부족 때문에 병사 모집이 치명타를 입었습니다." },
    {TR_BUILDING_LEGION_FOOD_WARNING_1, "최근의 식량 부족 때문에 병사들의 사기가 저하됐습니다"},
    {TR_BUILDING_LEGION_FOOD_WARNING_2, "최근의 식량 부족 때문에 병사들의 사기가 크게 저하됐습니다"},
    {TR_BUILDING_LEGION_STARVING, "식량 부족" },
    {TR_ADVISOR_LEGION_FOOD_SATISFIED, "병사들은 필요로 하는 식량을 모두 갖고 있습니다."},
    {TR_ADVISOR_LEGION_FOOD_NEEDED, "병사들은 보다 많은 식량이 필요합니다." },
    {TR_ADVISOR_LEGION_FOOD_CRITICAL, "병사들이 굶주립니다!"},
    {TR_ADVISOR_LEGION_MONTHS_FOOD_STORED, "보급창의 식량이 다하기까지 남은 달 수 :" },
    {TR_CITY_MESSAGE_TITLE_MESS_HALL_NEEDS_FOOD, "굶주린 병사들" },
    {TR_CITY_MESSAGE_TEXT_MESS_HALL_NEEDS_FOOD, "당신의 보급창이 심각한 식량 부족 사태에 직면했으며 병사들의 사기가 흔들립니다. 보급창이 재고가 충분한 곡물창고에서 제대로 식량을 공급받을 수 있게 하십시오"},
    {TR_CITY_MESSAGE_TEXT_MESS_HALL_MISSING, "당신의 도시가 보급창을 잃어 부대들이 굶주립니다. 즉시 보급창을 건설하십시오."},
    {TR_MARKET_SPECIAL_ORDERS_HEADER, "수집할 물품"},
    {TR_WARNING_NO_MESS_HALL, "먼저 병사들을 먹일 보급창을 건설해야 합니다." },
    {TR_WARNING_MAX_GRAND_TEMPLES, "최대 2개의 대성전만을 건설할 수 있습니다." },
    {TR_CITY_MESSAGE_TITLE_GRAND_TEMPLE_COMPLETE, "대성전 완성"},
    {TR_CITY_MESSAGE_TEXT_GRAND_TEMPLE_COMPLETE, "성직자와 신도들이 새 대성전의 봉헌 의식을 지켜보기 위해 몰려듭니다. 시민들이 경외심에 가득 차 당신의 업적을 올려다보고 있으며, 당신이 선택한 신이 당신에게 은총을 맘껏 내려줍니다."},
    {TR_CITY_MESSAGE_TITLE_MERCURY_BLESSING, "머큐리의 은총" },
    {TR_CITY_MESSAGE_TEXT_MERCURY_BLESSING, "당신의 헌신에 기뻐하며 머큐리는 당신의 산업에 축복을 내려 장인들이 쓸 원자재를 찾아줬습니다."},
    {TR_FIGURE_TYPE_WORK_CAMP_WORKER, "십장"},
    {TR_FIGURE_TYPE_WORK_CAMP_SLAVE, "짐꾼"},
    {TR_FIGURE_TYPE_WORK_CAMP_ARCHITECT, "기술자"},
    {TR_FIGURE_TYPE_MESS_HALL_SUPPLIER, "병참장교"},
    {TR_FIGURE_TYPE_MESS_HALL_COLLECTOR, "야영지 노예"},
    {TR_BUILDING_CERES_TEMPLE_MODULE_DESC, "대성전은 케레스의 성직자들을 조직해 식량과 기름을 배고픈 시민들에게 나눠주도록 합니다."},
    {TR_BUILDING_VENUS_TEMPLE_MODULE_DESC, "대성전은 비너스의 성직자들이 성스러운 포도주를 시민들에게 공급할 수 있게 합니다." },
    {TR_BUILDING_MARS_TEMPLE_MODULE_DESC, "대성전은 마르스의 성직자들이 식량을 보급창에 공급할 수 있게 합니다."},
    {TR_BUILDING_SMALL_TEMPLE_CERES_NAME, "케레스 신전"},
    {TR_BUILDING_SMALL_TEMPLE_NEPTUNE_NAME, "넵튠 신전"},
    {TR_BUILDING_SMALL_TEMPLE_MERCURY_NAME, "머큐리 신전"},
    {TR_BUILDING_SMALL_TEMPLE_MARS_NAME, "마르스 신전"},
    {TR_BUILDING_SMALL_TEMPLE_VENUS_NAME, "비너스 신전"},
    {TR_FIGURE_TYPE_PRIEST_SUPPLIER, "성직자" },
    // Transcription of mission_exact4.wav
    {TR_PHRASE_FIGURE_MISSIONARY_EXACT_4, "\"이 야만인들을 진정시킬 수 있도록 최선을 다하겠습니다. 분명 대화를 나눠보면 이들도 도시를 공격하는 걸 멈출 겁니다.\"" },
    {TR_CITY_MESSAGE_TITLE_PANTHEON_FESTIVAL, "연례 축제"},
    {TR_CITY_MESSAGE_TEXT_PANTHEON_FESTIVAL_CERES, "신앙 깊은 이들이 케레스를 기리는 케레알리아 축제를 위해 신전으로 모여듭니다. 농부들은 미래의 풍작을 기원하며 제물을 바칩니다."},
    {TR_CITY_MESSAGE_TEXT_PANTHEON_FESTIVAL_NEPTUNE, "오늘은 넵튠을 기리는 넵투날리아 축제 날입니다. 신도들은 나뭇가지와 이파리로 지은 오두막 밑에 모여들어 찌는 듯한 한여름 볕 속에 즐겁게 뛰어놉니다."},
    {TR_CITY_MESSAGE_TEXT_PANTHEON_FESTIVAL_MERCURY, "상인과 교역상들이 메르쿠랄리아 축제를 위해 숲으로 몰려 나옵니다. 머큐리의 신성한 축복을 빌며 배와 창고에는 성수가 뿌려집니다."},
    {TR_CITY_MESSAGE_TEXT_PANTHEON_FESTIVAL_MARS, "시민들은 도시 밖에 모여 마르스의 은총을 구하는 에퀴리아 축제를 기념합니다. 신도들이 마르스를 기리며 전차경기를 열자 주변은 천둥 같은 말발굽과 전차의 삐걱대는 소리들로 가득 찹니다."},
    {TR_CITY_MESSAGE_TEXT_PANTHEON_FESTIVAL_VENUS, "신도들은 비너스의 성스러운 날인 베네랄리아를 기념합니다. 시민들은 마음 깊은 곳의 문제들에 대해 앞으로 비너스가 은총을 내려줄 것을 구합니다."},
    {TR_TOOLTIP_BUTTON_DELETE_READ_MESSAGES, "이미 읽은 메시지 삭제"},
    {TR_TOOLTIP_BUTTON_MOTHBALL_ON, "이 건물 비활성화"},
    {TR_TOOLTIP_BUTTON_MOTHBALL_OFF, "이 건물 활성화"},
    {TR_TOOLTIP_BUTTON_ACCEPT_MARKET_LADIES, "시장 아줌마들이 이 건물에서 구매하는 것을 허용한다"},
    {TR_TOOLTIP_BUTTON_ACCEPT_TRADE_CARAVAN, "무역상들이 이 건물에서 무역하는 것을 허용한다"},
    {TR_TOOLTIP_BUTTON_ACCEPT_TRADE_SHIPS, "무역선들이 이 건물에서 무역하는 것을 허용한다"},
    {TR_CONFIG_HEADER_CITY_MANAGEMENT_CHANGES, "도시 관리"},
    {TR_BUILDING_LIGHTHOUSE, "등대"},
    {TR_BUILDING_LIGHTHOUSE_PHASE_1, "(토대)" },
    {TR_BUILDING_LIGHTHOUSE_PHASE_2, "(기단)" },
    {TR_BUILDING_LIGHTHOUSE_PHASE_3, "(탑)" },
    {TR_BUILDING_LIGHTHOUSE_PHASE_4, "(등)" },
    {TR_BUILDING_LIGHTHOUSE_PHASE_1_TEXT, "기술자들이 커다란 암석 탑의 하중도 버텨낼 든든한 토대를 쌓는 중입니다." },
    {TR_BUILDING_LIGHTHOUSE_PHASE_2_TEXT, "석공들이 수평선 너머에서도 보이는 불빛을 받쳐올릴 받침을 건축하는 중입니다." },
    {TR_BUILDING_LIGHTHOUSE_PHASE_3_TEXT, "석공들이 기술을 유감없이 발휘하자 등대 탑이 매일같이 더더욱 높이 솟고 있습니다." },
    {TR_BUILDING_LIGHTHOUSE_PHASE_4_TEXT, "기술자들이 등대에 최후의 마무리를 하고 있습니다. 곧 저 멀리까지 빛을 뿜으며 배들을 집으로 인도할 것입니다." },
    {TR_BUILDING_LIGHTHOUSE_CONSTRUCTION_DESC, "등대를 건축하는 데는 창고에 비축된 원자재, 노동자 숙소의 노동자들, 기술자 길드의 기술자들이 필요합니다." },
    {TR_BUILDING_LIGHTHOUSE_BONUS_DESC, "어선이 10% 빠르게 움직이며, 바다 폭풍 지속시간이 반으로 감소합니다." },
    {TR_EDITOR_ALLOWED_BUILDINGS_MONUMENTS, "기념물"},
    {TR_CITY_MESSAGE_TEXT_LIGHTHOUSE_COMPLETE, "완성된 등대는 위엄 서린 돌기둥이 되어 수평선에 떠오릅니다. 세상 끝나는 날까지 이 등대가 배들을 집으로 인도할 수 있기를." },
    {TR_CITY_MESSAGE_TEXT_PANTHEON_COMPLETE, "판테온이 완성되었습니다. 신들과 로마 시민들의 힘을 드러내는 이 기념물에 비길 만한 것은 없습니다." },
    {TR_CITY_MESSAGE_TITLE_MONUMENT_COMPLETE, "기념물 완성" },
    {TR_CITY_MESSAGE_TITLE_NEPTUNE_BLESSING, "넵튠의 축복"},
    {TR_CITY_MESSAGE_TEXT_NEPTUNE_BLESSING, "당신 도시의 헌신에 보답하고자, 넵튠이 무역상들의 순탄한 여행길을 12개월 간 보장하며 그 동안은 수출하는 물품이 원래 가격의 150%를 받을 것입니다."},
    {TR_CITY_MESSAGE_TITLE_VENUS_BLESSING, "비너스의 축복" },
    {TR_CITY_MESSAGE_TEXT_VENUS_BLESSING, "자신에게 바쳐진 행복한 숭배의식에 기뻐한 비너스가 젊음, 건강, 행복을 당신의 백성에게 베풀어 노동 인구의 규모를 늘려줍니다." },
    {TR_BUILDING_MENU_STATUES, "석상" },
    {TR_BUILDING_MENU_GOV_RES, "통치자의 주택" },
    {TR_OVERLAY_ROADS, "도로" },
    {TR_NO_EXTRA_ASSETS_TITLE, "모드 폴더를 찾을 수 없습니다" },
    {TR_NO_EXTRA_ASSETS_MESSAGE,
        "당신의 시저 3이 설치된 위치에 모드 폴더가 올바르게 설치되지 않았습니다."
        "새 요소들이 정상적으로 표시되지 않을 것입니다.\n"
        "'/assets' 폴더가 시저 3 폴더에 정확히 위치하도록 설치해 주십시오." },
    {TR_WARNING_WATER_NEEDED_FOR_LIGHTHOUSE, "등대는 물 근처에 설치되어야 합니다"},
    {TR_TOOLTIP_OVERLAY_PANTHEON_ACCESS, "이 집은 판테온의 모든 5가지 신의 신앙을 접할 수 있습니다"},
    {TR_BUILDING_LEGION_FOOD_BONUS, "풍부한 식량이 사기를 고양시키고 있습니다" },
    {TR_BUILDING_LEGION_FOOD_STATUS, "식량 상태"},
    {TR_TOOLTIP_BUTTON_ACCEPT_QUARTERMASTER, "병참장교에게 여기서 식량을 가져가도록 허가"},
    {TR_WARNING_RESOURCES_NOT_AVAILABLE, "필요로 하는 원자재를 구할 수 없습니다"},
    {TR_CONFIG_GP_CH_MONUMENTS_BOOST_CULTURE_RATING, "완성된 대성전이 문화 등급에 +8을 부여"},
    {TR_BUTTON_BACK_TO_MAIN_MENU, "메인 메뉴로 돌아가기" },
    {TR_LABEL_PAUSE_MENU, "정지" },
    {TR_OVERLAY_LEVY, "유지비" },
    {TR_TOOLTIP_OVERLAY_LEVY, " 자금이 다달이 유지비로 들어감." }, 
    {TR_MAP_EDITOR_OPTIONS, "시나리오 설정" },
    {TR_BUILDING_MARS_TEMPLE_MODULE_DESC_NO_MESS, "이 신전은 건설된 후 보급창에 공급할 식량을 수집할 것입니다." },
    {TR_BUTTON_GO_TO_SITE, "위치로 이동" },
    {TR_RETURN_ALL_TO_FORT, "모두"},
    {TR_HOTKEY_DUPLICATE_TITLE, "단축키가 이미 사용 중"},
    {TR_HOTKEY_DUPLICATE_MESSAGE, "해당 키 조합은 이미 다음의 행동에 할당되어 있습니다:"},
    {TR_CONFIG_DIGIT_SEPARATOR, "Separate digits"},
    {TR_WARNING_SCREENSHOT_SAVED, "스크린샷이 저장되었습니다: "} // TODO: Google translate
};

void translation_korean(const translation_string **strings, int *num_strings)
{
    *strings = all_strings;
    *num_strings = sizeof(all_strings) / sizeof(translation_string);
}