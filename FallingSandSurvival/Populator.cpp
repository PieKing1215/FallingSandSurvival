
//#include "Populator.h"
//#include "Textures.h"
//#include <string> 
//#include "Structures.h"
//
//std::vector<PlacedStructure> Populator::apply(MaterialInstance* tiles, Chunk ch, World world){
//	for (int x = 0; x < CHUNK_W; x++) {
//		for (int y = 0; y < CHUNK_H; y++) {
//			int px = x + ch.x * CHUNK_W;
//			int py = y + ch.y * CHUNK_H;
//			if (tiles[x + y * CHUNK_W].mat.physicsType == PhysicsType::SOLID && tiles[x + y * CHUNK_W].mat.id != Materials::CLOUD.id) {
//				double n = world.perlin.noise(px / 64.0, py / 64.0, 3802);
//				double n2 = world.perlin.noise(px / 150.0, py / 150.0, 6213);
//				double ndetail = world.perlin.noise(px / 16.0, py / 16.0, 5319) * 0.1;
//				if (n2 + n + ndetail < std::fmin(0.95, (py) / 1000.0)) {
//					double nlav = world.perlin.noise(px / 250.0, py / 250.0, 7018);
//					if (nlav > 0.7) {
//						tiles[x + y * CHUNK_W] = rand() % 3 == 0 ? (ch.y > 5 ? Tiles::createLava() : Tiles::createWater()) : Tiles::NOTHING;
//					}
//					else {
//						tiles[x + y * CHUNK_W] = Tiles::NOTHING;
//					}
//				}
//				else {
//					double n3 = world.perlin.noise(px / 64.0, py / 64.0, 9828);
//					if (n3 - 0.25 > py / 1000.0) {
//						tiles[x + y * CHUNK_W] = Tiles::NOTHING;
//					}
//				}
//			}
//
//			if (tiles[x + y * CHUNK_W].mat.id == Materials::SMOOTH_STONE.id) {
//				double n = world.perlin.noise(px / 48.0, py / 48.0, 5124);
//				if (n < 0.25) tiles[x + y * CHUNK_W] = Tiles::createIron(px, py);
//			}
//
//			if (tiles[x + y * CHUNK_W].mat.id == Materials::SMOOTH_STONE.id) {
//				double n = world.perlin.noise(px / 32.0, py / 32.0, 7513);
//				if (n < 0.20) tiles[x + y * CHUNK_W] = Tiles::createGold(px, py);
//			}
//
//			MaterialInstance prop = tiles[x + y * CHUNK_W];
//			if (prop.mat.id == Materials::SMOOTH_STONE.id) {
//				int dist = 6 + world.perlin.noise(px / 10.0, py / 10.0, 3323) * 5 + 5;
//				for (int dx = -dist; dx <= dist; dx++) {
//					for (int dy = -dist; dy <= dist; dy++) {
//						if (x + dx >= 0 && x + dx < CHUNK_W && y + dy >= 0 && y + dy < CHUNK_H) {
//							if (tiles[(x + dx) + (y + dy) * CHUNK_W].mat.physicsType == PhysicsType::AIR || (tiles[(x + dx) + (y + dy) * CHUNK_W].mat.physicsType == PhysicsType::SAND && tiles[(x + dx) + (y + dy) * CHUNK_W].mat.id != Materials::SOFT_DIRT.id)) {
//								tiles[x + y * CHUNK_W] = Tiles::createCobbleStone(px, py);
//								goto nextTile;
//							}
//						}
//					}
//				}
//			}
//			else if (prop.mat.id == Materials::SMOOTH_DIRT.id) {
//				int dist = 6 + world.perlin.noise(px / 10.0, py / 10.0, 3323) * 5 + 5;
//				for (int dx = -dist; dx <= dist; dx++) {
//					for (int dy = -dist; dy <= dist; dy++) {
//						if (x + dx >= 0 && x + dx < CHUNK_W && y + dy >= 0 && y + dy < CHUNK_H) {
//							if (tiles[(x + dx) + (y + dy) * CHUNK_W].mat.physicsType == PhysicsType::AIR || (tiles[(x + dx) + (y + dy) * CHUNK_W].mat.physicsType == PhysicsType::SAND && tiles[(x + dx) + (y + dy) * CHUNK_W].mat.id != Materials::SOFT_DIRT.id)) {
//								tiles[x + y * CHUNK_W] = Tiles::createCobbleDirt(px, py);
//								goto nextTile;
//							}
//						}
//					}
//				}
//			}
//
//		nextTile: {}
//
//		}
//	}
//
//	std::vector<PlacedStructure> structs;
//	//if (ch.x % 2 == 0) {
//	//	TileProperties* str = new TileProperties[100 * 50];
//	//	for (int x = 0; x < 20; x++) {
//	//		for (int y = 0; y < 8; y++) {
//	//			str[x + y * 20] = Tiles::TEST_SOLID;
//	//		}
//	//	}
//	//	PlacedStructure* ps = new PlacedStructure(Structure(20, 8, str), ch.x * CHUNK_W - 10, ch.y * CHUNK_H - 4);
//	//	//world.addParticle(new Particle(Tiles::TEST_SAND, ch.x * CHUNK_W, ch.y * CHUNK_H, 0, 0, 0, 1));
//	//	structs.push_back(*ps);
//	//	//std::cout << "placestruct " << world.structures.size() << std::endl;
//	//}
//
//	//if (ch.x % 2 == 0) {
//	//	TileProperties* str = new TileProperties[100 * 50];
//	//	for (int x = 0; x < 100; x++) {
//	//		for (int y = 0; y < 50; y++) {
//	//			if (x == 0 || x == 99 || y == 0 || y == 49) { 
//	//				str[x + y * 100] = Tiles::TEST_SOLID;
//	//			}else {
//	//				str[x + y * 100] = Tiles::NOTHING;
//	//			}
//	//		}
//	//	}
//	//	PlacedStructure* ps = new PlacedStructure(Structure(100, 50, str), ch.x * CHUNK_W - 50, ch.y * CHUNK_H - 25);
//	//	//world.addParticle(new Particle(Tiles::TEST_SAND, ch.x * CHUNK_W, ch.y * CHUNK_H, 0, 0, 0, 1));
//	//	structs.push_back(*ps);
//	//	//std::cout << "placestruct " << world.structures.size() << std::endl;
//	//}
//
//	if (ch.y < 2 && rand() % 2 == 0) {
//		//TileProperties* str = new TileProperties[100 * 50];
//		int posX = ch.x * CHUNK_W + (rand() % CHUNK_W);
//		int posY = ch.y * CHUNK_H + (rand() % CHUNK_H);
//		/*for (int x = 0; x < 100; x++) {
//			for (int y = 0; y < 50; y++) {
//				str[x + y * 100] = Tiles::createCloud(x + posX + ch.x * CHUNK_W, y + posY + ch.y * CHUNK_H);
//			}
//		}*/
//		std::string m = "assets/objects/cloud_";
//		m.append(std::to_string(rand() % 11));
//		m.append(".png");
//		Structure st = Structure(Textures::loadTexture(m, SDL_PIXELFORMAT_ARGB8888), Materials::CLOUD);
//		PlacedStructure* ps = new PlacedStructure(st, posX, posY);
//		//world.addParticle(new Particle(Tiles::TEST_SAND, ch.x * CHUNK_W, ch.y * CHUNK_H, 0, 0, 0, 1));
//		structs.push_back(*ps);
//		//std::cout << "placestruct " << world.structures.size() << std::endl;
//	}
//
//	float treePointsScale = 2000;
//	std::vector<b2Vec2> treePts = world.getPointsWithin((ch.x - 1) * CHUNK_W / treePointsScale, (ch.y - 1) * CHUNK_H / treePointsScale, CHUNK_W * 3 / treePointsScale, CHUNK_H * 3 / treePointsScale);
//	Structure tree = Structures::makeTree1(world, ch.x * CHUNK_W, ch.y * CHUNK_H);
//	std::cout << treePts.size() << std::endl;
//	for (int i = 0; i < treePts.size(); i++) {
//		int px = treePts[i].x * treePointsScale - ch.x * CHUNK_W;
//		int py = treePts[i].y * treePointsScale - ch.y * CHUNK_H;
//		
//		for (int xx = 0; xx < tree.w; xx++) {
//			for (int yy = 0; yy < tree.h; yy++) {
//				if (px + xx >= 0 && px + xx < CHUNK_W && py + yy >= 0 && py + yy < CHUNK_H) {
//					if (tree.tiles[xx + yy * tree.w].mat.physicsType != PhysicsType::AIR) {
//						tiles[(px + xx) + (py + yy) * CHUNK_W] = tree.tiles[xx + yy * tree.w];
//					}
//				}
//			}
//		}
//	}
//
//	return structs;
//}
