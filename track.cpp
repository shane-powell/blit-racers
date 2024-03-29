#include "track.hpp"
#include "tiledata.hpp"
using namespace util;

Track* LoadTrack(uint8_t trackId)
{
	switch (trackId)
	{
	case 0:
		return new Track(8, const_cast<uint8_t*>(map1), 128, 128, const_cast<uint8_t*>(tile_sheet_1), const_cast<uint8_t*>(tile_sheet_1), const_cast<uint8_t*>(tile_sheet_1),
			// Start locations
			{
				Position(blit::Point(270,480),180),
				Position(blit::Point(305,480),180),
				Position(blit::Point(270,520),180),
				Position(blit::Point(310,520),180)
			},
			// Nodes
			{
				blit::Point(270, 440),
				blit::Point(270, 412),
				blit::Point(270, 346),
				blit::Point(286, 297),
				blit::Point(331,288),
				blit::Point(405, 287),
				blit::Point(486, 305),
				blit::Point(555, 334),
				blit::Point(636, 300),
				blit::Point(699,283),
				blit::Point(726, 319),
				blit::Point(720, 394),
				blit::Point(728, 477),
				blit::Point(653,526),
				blit::Point(589, 550),
				blit::Point(498, 557),
				blit::Point(399, 548),
				blit::Point(302, 536),
				blit::Point(257,501),
				blit::Point(262, 469)
			}, const_cast<uint8_t*>(map1_preview), "Kitchen", const_cast<uint8_t*>(car1),
			// Checkpoint locations
			{
				Position(blit::Point(280, 448),180),
				Position(blit::Point(288, 304),240),
				Position(blit::Point(504, 320),270),
				Position(blit::Point(656, 280),270),
				Position(blit::Point(741, 417),360),
				//Position(blit::Point(728, 424),90),
				Position(blit::Point(672, 536),90),
				Position(blit::Point(536, 536),90),
				Position(blit::Point(368, 544),90)
			},
			blit::Size(24, 24), 
			{
				//tomato
				TileData(36,  false, true, false),
				TileData(37,  false, true, false),
				TileData(38,  false, true, false),
				TileData(39,  false, true, false),
				TileData(52,  false, true, false),
				TileData(53,  false, true, false),
				TileData(54,  false, true, false),
				TileData(55,  false, true, false),
				TileData(68,  false, true, false),
				TileData(69,  false, true, false),
				TileData(70,  false, true, false),
				TileData(71,  false, true, false),
				TileData(84,  false, true, false),
				TileData(85,  false, true, false),
				TileData(86,  false, true, false),
				TileData(87,  false, true, false),

				//Floor
				TileData(10,  false, false, true),
				TileData(11,  false, false, true),
				TileData(12,  false, false, true),
				TileData(13,  false, false, true),
				TileData(26,  false, false, true),
				TileData(27,  false, false, true),
				TileData(28,  false, false, true),
				TileData(29,  false, false, true)
			}, Vehicle::car);
	case 1:
		return new Track(8, const_cast<uint8_t*>(map2), 128, 128, const_cast<uint8_t*>(tile_sheet_2), const_cast<uint8_t*>(tile_sheet_2), const_cast<uint8_t*>(tile_sheet_2),
			// Start locations
			{
				Position(blit::Point(270,480),180),
				Position(blit::Point(305,480),180),
				Position(blit::Point(270,520),180),
				Position(blit::Point(310,520),180)
			},
			// Nodes
			{
				blit::Point(270, 440),
				blit::Point(253, 247),
				blit::Point(448, 174),
				blit::Point(715, 182),
				blit::Point(822,261),
				blit::Point(818, 763),
				blit::Point(670, 797),
				blit::Point(660, 578),
				blit::Point(625, 313),
				blit::Point(516, 286),
				blit::Point(446, 424),
				blit::Point(441, 628),
				blit::Point(417, 816),
				blit::Point(199,766),
				blit::Point(218, 574)
				/*blit::Point(844, 801),
				blit::Point(843, 202),
				blit::Point(799, 157),
				blit::Point(199, 155),
				blit::Point(161, 185),
				blit::Point(152, 346),
				blit::Point(166, 438),
				blit::Point(166, 599),
				blit::Point(154, 621),
				blit::Point(151, 856),
				blit::Point(192, 882),
				blit::Point(237, 871),
				blit::Point(276, 796)*/
			}, const_cast<uint8_t*>(map2_preview), "Bath tub", const_cast<uint8_t*>(boat1),
			// Checkpoint locations
			{
				Position(blit::Point(280, 448),0),
				Position(blit::Point(288, 304),0),
				Position(blit::Point(504, 320),0),
				Position(blit::Point(656, 280),0),
				Position(blit::Point(288, 304),0),
				Position(blit::Point(728, 424),0),
				Position(blit::Point(672, 536),0),
				Position(blit::Point(536, 536),0),
				Position(blit::Point(368, 544),0)
			}, blit::Size(16, 16),
			{
				//Bubbles
				TileData(10,  false, true, false),
				TileData(11,  false, true, false),
				TileData(12,  false, true, false),
				TileData(13,  false, true, false),
				TileData(14,  false, true, false),
				TileData(15,  false, true, false),
				TileData(27,  false, true, false),
				TileData(28,  false, true, false),
				TileData(29,  false, true, false),
				TileData(30,  false, true, false),
				TileData(31,  false, true, false),
				TileData(32,  false, true, false),
				TileData(46,  false, true, false),
				TileData(47,  false, true, false),
				TileData(62,  false, true, false),
				TileData(63,  false, true, false),

				//Duck
				TileData(32,  false, true, false),
				TileData(33,  false, true, false),
				TileData(34,  false, true, false),
				TileData(35,  false, true, false),
				TileData(36,  false, true, false),

				TileData(48,  false, true, false),
				TileData(49,  false, true, false),
				TileData(50,  false, true, false),
				TileData(51,  false, true, false),
				TileData(52,  false, true, false),

				TileData(64,  false, true, false),
				TileData(65,  false, true, false),
				TileData(66,  false, true, false),
				TileData(67,  false, true, false),
				TileData(68,  false, true, false),

				TileData(80,  false, true, false),
				TileData(81,  false, true, false),
				TileData(82,  false, true, false),
				TileData(83,  false, true, false),
				TileData(84,  false, true, false),

				TileData(96,  false, true, false),
				TileData(97,  false, true, false),
				TileData(98,  false, true, false),
				TileData(99,  false, true, false),
				TileData(100,  false, true, false),

				//Soap
				TileData(201,  false, false, false, true),
				TileData(202,  false, false, false, true),
				TileData(203,  false, false, false, true),
				TileData(204,  false, false, false, true),
				TileData(205,  false, false, false, true),
				TileData(205,  false, false, false, true),
				TileData(207,  false, false, false, true),

				TileData(217,  false, false, false, true),
				TileData(218,  false, false, false, true),
				TileData(219,  false, false, false, true),
				TileData(220,  false, false, false, true),
				TileData(221,  false, false, false, true),
				TileData(222,  false, false, false, true),
				TileData(223,  false, false, false, true),

				TileData(233,  false, false, false, true),
				TileData(234,  false, false, false, true),
				TileData(235,  false, false, false, true),
				TileData(236,  false, false, false, true),
				TileData(237,  false, false, false, true),
				TileData(238,  false, false, false, true),
				TileData(239,  false, false, false, true),

				TileData(249,  false, false, false, true),
				TileData(250,  false, false, false, true),
				TileData(251,  false, false, false, true),
				TileData(252,  false, false, false, true),
				TileData(253,  false, false, false, true),
				TileData(254,  false, false, false, true),
				TileData(255,  false, false, false, true)

			}, Vehicle::boat);
		break;
	}

}