#include "MyScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "HelloWorldScene.h"
#include "SecondScene.h"
#include "math.h"

USING_NS_CC;

using namespace cocostudio::timeline;

Scene* SecondScene::createScene()
{
	//物理引擎
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2(0, -300));
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
	scene->getPhysicsWorld()->setSubsteps(10);

	auto layer = SecondScene::create();

	scene->addChild(layer);

	return scene;
}

bool SecondScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}
	//预加载音乐音效
	myMusicer = CocosDenshion::SimpleAudioEngine::getInstance();
	myMusicer->preloadBackgroundMusic("bgMusic.mp3");
	myMusicer->preloadBackgroundMusic("button.WAV");
	myMusicer->preloadBackgroundMusic("ready.mp3");
	myMusicer->preloadBackgroundMusic("boom.wav");
	myMusicer->preloadBackgroundMusic("round2.mp3");
	myMusicer->preloadBackgroundMusic("fire1.mp3");
	myMusicer->preloadBackgroundMusic("fire2.mp3");
	myMusicer->preloadBackgroundMusic("dead.mp3");
	myMusicer->preloadBackgroundMusic("star.mp3");
	myMusicer->preloadBackgroundMusic("pass.mp3");

	//瓦片地图
	auto map1 = TMXTiledMap::create("map1.tmx");
	addChild(map1, 0, "map1");

	//创建精灵 --泡泡
	auto player = Sprite::create("player.png");
	player->setTag(1);
	player->setPosition(20, 580);
	addChild(player, 0, "player");

	//泡泡出场
	auto playerIn = MoveTo::create(2, Vec2(50, 580));//泡泡移动
	auto blink = Blink::create(3.0, 5);//3秒内闪烁5次
	auto seq0 = Sequence::create(playerIn, blink, NULL);//按顺序执行
	player->runAction(seq0);//绑定泡泡

	//星星1
	auto star1 = Sprite::create("star.png");
	auto blink1 = Blink::create(60,60);
	star1->runAction(blink1);
	star1->setScale(0.015);
	star1->setPosition(440, 536);
	addChild(star1, 0, "star1");

	//星星2
	auto star2 = Sprite::create("star.png");
	star2->setScale(0.02);
	star2->setPosition(860, 600);
	addChild(star2, 0, "star2");

	//得分
	auto label0 = Label::create("X 0", "Arial", 30);
	label0->setPosition(910, 600);
	addChild(label0, 0, "label0");

	//创建精灵 --障碍物
	auto land = Sprite::create("land.png");
	land->setPosition(300, 530);
	addChild(land, 0, "land");

	//障碍物巡逻
	auto move = MoveBy::create(3, Vec2(250, 0));
	auto delay1 = DelayTime::create(1);
	auto seq1 = Sequence::create(move, delay1, move->reverse(), delay1, NULL);
	auto repseq = RepeatForever::create(seq1);//无限循环
	land->runAction(repseq);

	//按钮
	auto button = Button::create("button.png");
	button->setScale(0.5);
	button->setPosition(ccp(808, 150));
	this->addChild(button,0,"button");

	//创建标签
	auto label = Label::createWithTTF("Round Two", "FeiGunGun.ttf", 40);
	auto ls = label->getContentSize();
	label->setPosition(-ls.width / 2, 320);
	addChild(label);

	//标签移动
	auto moveIn = MoveTo::create(2.0, Vec2(480, 320));//移动标签至中心
	auto moveOut = MoveTo::create(2.0, Vec2(1200, 320));//移出
	auto delay = DelayTime::create(1.0);//停留1秒
	auto seq = Sequence::create(moveIn, delay, moveOut, NULL);//按顺序执行
	label->runAction(seq);//绑定标签
	this->scheduleOnce(schedule_selector(SecondScene::RoundTwo), 0.8f);//ready音效定时器
	this->scheduleOnce(schedule_selector(SecondScene::Ready), 2.8f);//ready音效定时器
	this->scheduleOnce(schedule_selector(SecondScene::Start), 5.0f);//背景音乐定时器

	scheduleUpdate();

	return true;
}

//定时器 - 1. 移动定时器 - 障碍物检测函数
bool SecondScene::IsCanReach(float x, float y)
{
	auto map1 = (TMXTiledMap*)getChildByName("map1");
	auto mask = map1->layerNamed("mask");//获取障碍物图层
	auto TileSize = 16;
	int mapX = x / TileSize;
	int col = map1->getMapSize().height;
	int mapY = col - y / 16;
	if (mask->getTileAt(Vec2(mapX, mapY)))
		return true;
	return false;
}

//定时器 - 1. 移动定时器 - 过关检测函数
bool SecondScene::IsCanPass(float x, float y)
{
	auto map1 = (TMXTiledMap*)getChildByName("map1");
	auto pass = map1->layerNamed("pass");
	auto TileSize = 16;
	int mapX = x / TileSize;
	int col = map1->getMapSize().height;
	int mapY = col - y / 16;
	if (pass->getTileAt(Vec2(mapX, mapY)))
		return true;
	return false;
}

//定时器 - 1. 移动定时器 - 星星检测函数
bool SecondScene::IsCanGet(float x, float y)
{
	auto map1 = (TMXTiledMap*)getChildByName("map1");
	auto star = map1->layerNamed("star");
	auto TileSize = 16;
	int mapX = x / TileSize;
	int col = map1->getMapSize().height;
	int mapY = col - y / 16;
	if (star->getTileAt(Vec2(mapX, mapY)))
		return true;
	return false;
}

//获取最小值
int SecondScene::min(float x, float y)
{
	if (x > y)
		return y;
	else
		return x;
}

//获取最大值
int SecondScene::max(float x, float y)
{
	if (x > y)
		return x;
	else
		return y;
}

//播放round2音效
void SecondScene::RoundTwo(float){
	myMusicer->playBackgroundMusic("round2.mp3");
}

//播放ready音效
void SecondScene::Ready(float){
	myMusicer->playBackgroundMusic("ready.mp3");
}

//烟花音效
void SecondScene::fire1(float){
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("fire1.mp3");
}

void SecondScene::fire2(float){
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("fire2.mp3");
}

//游戏开始
void SecondScene::Start(float){
	//播放背景音乐
	myMusicer->playBackgroundMusic("bgMusic.mp3");

	//泡泡刚体
	auto player = getChildByName("player");
	playerbody = PhysicsBody::createCircle(20);
	playerbody->applyForce(Vec2(500, 0));
	player->setPhysicsBody(playerbody);

	//按钮
	auto button = Button::create("button.png");
	button->addTouchEventListener(CC_CALLBACK_2(SecondScene::touchMove, this));
	button->setScale(0.5);
	button->setPressedActionEnabled(true);
	button->setPosition(ccp(808, 150));
	this->addChild(button,0,"button1");

	//按钮监听器
	auto listener = EventListenerTouchOneByOne::create();
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

//执行移动
void SecondScene::toch_Move(float dt)
{
	if (getChildByTag(1))
		playerbody->applyImpulse(Vec2(0, 1000));
}

//长按按钮事件
void SecondScene::touchMove(cocos2d::Ref* pSender, Widget::TouchEventType event)
{
	if (event == Widget::TouchEventType::BEGAN)
	{
		this->schedule(schedule_selector(SecondScene::toch_Move), 0.0f);
	}

	if (event == Widget::TouchEventType::ENDED)
	{
		this->unschedule(schedule_selector(SecondScene::toch_Move));
	}
}

//定时器 - 碰撞检测
void SecondScene::update(float dt)
{
	auto player = getChildByName("player");
	auto land = getChildByName("land");
	auto star1 = getChildByName("star1");
	auto label0 = getChildByName("label0");

	//圆和矩形碰撞算法
	int x = (player->getPositionX()) - (land->getPositionX());
	int y = (player->getPositionY()) - (land->getPositionY());
	int minX = min(x, 24);
	int maxX = max(minX, -24);
	int minY = min(y, 16);
	int maxY = max(minY, -16);

	//障碍物碰撞检测
	if (((maxX - x)*(maxX - x) + (maxY - y) *(maxY - y) <= 400) || IsCanReach(player->getPositionX() + 20, player->getPositionY() + 20) || IsCanReach(player->getPositionX() - 20, player->getPositionY() - 20))
	{	
		//游戏完善
		//unschedule("move");
		land->stopAllActions();
		//listener->setEnabled(false);

		//界面设计 - 声音 - 3. 主角爆炸声音
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("boom.wav");

		//动画设计 - 3. 爆炸动画
		auto SFC = SpriteFrameCache::getInstance();
		SFC->addSpriteFramesWithFile("boom.plist", "boom.png");
		auto animation = Animation::create();
		char name[20];
		for (int i = 0; i <= 15; i++)
		{
			sprintf(name, "boom%d.png", i);
			animation->addSpriteFrame(SFC->getSpriteFrameByName(name));
		}
		animation->setDelayPerUnit(0.1);
		animation->setLoops(1);
		auto animate = Animate::create(animation);
		auto explode = Sprite::createWithSpriteFrameName("boom0.png");
		explode->setPosition(player->getPosition());
		auto removeExplode = CallFunc::create(CC_CALLBACK_0(SecondScene::ExplodeRemove, this));
		auto seq = Sequence::create(animate, removeExplode, NULL);
		explode->runAction(seq);
		addChild(explode, 0, "explode");

		//销毁精灵
		unscheduleUpdate();
		removeChild(player);

		auto dict3 = Dictionary::createWithContentsOfFile("chn.xml");
		auto chn3 = ((String*)dict3->objectForKey("over"))->getCString();
		auto label3 = Label::createWithTTF(chn3, "FeiGunGun.ttf", 40);
		label3->setPosition(480, 400);
		addChild(label3);

		//弹出菜单
		ShowMenu();
		return;
	}

	//过关检测
	else if (IsCanPass(player->getPositionX() + 7, player->getPositionY() + 7) || IsCanPass(player->getPositionX() - 7, player->getPositionY() - 7))
	{
		if (!label0){
			//销毁精灵
			unscheduleUpdate();
			removeChild(player);

			//烟花
			this->scheduleOnce(schedule_selector(SecondScene::fire1), 0.0f);
			this->scheduleOnce(schedule_selector(SecondScene::fire2), 1.0f);

			//粒子系统  -  烟花
			/*auto particle = ParticleSystemQuad::create("fire.plist");
			particle->setPosition(480,320);
			addChild(particle);*/

			//动画设计 - 3. 烟花动画
			auto SFC1 = SpriteFrameCache::getInstance();
			SFC1->addSpriteFramesWithFile("fire.plist", "fire.png");
			auto animation1 = Animation::create();
			char name1[20];
			for (int i = 0; i <= 15; i++)
			{
				sprintf(name1, "fire%d.png", i);
				animation1->addSpriteFrame(SFC1->getSpriteFrameByName(name1));
			}
			animation1->setDelayPerUnit(0.1);
			animation1->setLoops(1);
			auto animate1 = Animate::create(animation1);
			auto explode1 = Sprite::createWithSpriteFrameName("fire0.png");
			explode1->setPosition(480,500);
			auto removeExplode1 = CallFunc::create(CC_CALLBACK_0(SecondScene::ExplodeRemove, this));
			auto fire = Sprite::create("fire0.png");
			fire->setPosition(480,-10);
			addChild(fire,0,"fire");
			auto firemove = MoveTo::create(1,Vec2(480,500));
			auto seq1 = Sequence::create(firemove, animate1, removeExplode1, NULL);
			fire->runAction(seq1);
			addChild(fire, 0, "explode1");
			this->scheduleOnce(schedule_selector(SecondScene::remove), 1.0f);

			//显示通关
			auto dict2 = Dictionary::createWithContentsOfFile("chn.xml");
			auto chn2 = ((String*)dict2->objectForKey("win"))->getCString();
			auto label2 = Label::createWithTTF(chn2, "FeiGunGun.ttf", 40);
			label2->setPosition(480, 430);
			addChild(label2);

			//弹出菜单
			ShowMenu1();
		}
		else
		{
			//销毁精灵
			unscheduleUpdate();
			removeChild(player);

			//播放死亡音乐
			myMusicer->playBackgroundMusic("dead.mp3");

			//显示失败
			auto dict2 = Dictionary::createWithContentsOfFile("chn.xml");
			auto chn2 = ((String*)dict2->objectForKey("lose"))->getCString();
			auto label2 = Label::createWithTTF(chn2, "FeiGunGun.ttf", 40);
			label2->setPosition(480, 450);
			addChild(label2);
			auto dict3 = Dictionary::createWithContentsOfFile("chn.xml");
			auto chn3 = ((String*)dict3->objectForKey("over"))->getCString();
			auto label3 = Label::createWithTTF(chn3, "FeiGunGun.ttf", 40);
			label3->setPosition(480, 400);
			addChild(label3);

			//弹出菜单
			ShowMenu();
		}
		return;
	}
	else if (IsCanGet(player->getPositionX(), player->getPositionY()))
	{
		//播放得分音乐
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("star.mp3");

		//销毁星星和得分0
		removeChild(star1);
		removeChild(label0);

		//得分1
		auto label1 = Label::create("X 1", "Arial", 30);
		label1->setPosition(910, 600);
		addChild(label1, 0, "label1");
		
		//unscheduleUpdate();
		return;
	}
}


//销毁爆炸
void SecondScene::ExplodeRemove()
{
	removeChildByName("explode");
	removeChildByName("explode1");
}

//销毁烟花
void SecondScene::remove(float)
{
	removeChildByName("fire");
}


//失败菜单
void SecondScene::ShowMenu()
{
	//播放死亡音乐
	myMusicer->playBackgroundMusic("dead.mp3");

	if (!getChildByName("menu"))
	{
		auto resumeLabel = Label::create("Resume", "Arial", 30);
		auto resumeMI = MenuItemLabel::create(resumeLabel, this, menu_selector(SecondScene::progress1));
		resumeMI->setPosition(-100, 0);
		auto exitLabel = Label::create("Exit", "Arial", 30);
		auto exitMI = MenuItemLabel::create(exitLabel, this, menu_selector(SecondScene::exit));
		exitMI->setPosition(100, 0);
		auto menu = Menu::create(resumeMI, exitMI, NULL);
		addChild(menu, 0, "menu");
	}
	else
		getChildByName("menu")->setVisible(true);
}

//通过菜单
void SecondScene::ShowMenu1()
{
	//播放过关音乐
	myMusicer->playBackgroundMusic("pass.mp3");

	//显示菜单
	if (!getChildByName("menu1"))
	{
		auto homeLabel = Label::create("Home", "Arial", 30);
		auto homeMI = MenuItemLabel::create(homeLabel, this, menu_selector(SecondScene::progress));
		homeMI->setPosition(-150, 0);
		auto resumeLabel = Label::create("Resume", "Arial", 30);
		auto resumeMI = MenuItemLabel::create(resumeLabel, this, menu_selector(SecondScene::progress1));
		resumeMI->setPosition(0, 0);
		auto exitLabel = Label::create("Exit", "Arial", 30);
		auto exitMI = MenuItemLabel::create(exitLabel, this, menu_selector(SecondScene::exit));
		exitMI->setPosition(150, 0);
		auto menu1 = Menu::create(homeMI, resumeMI,exitMI, NULL);
		addChild(menu1, 0, "menu1");
	}
	else
		getChildByName("menu1")->setVisible(true);
}

//过关进度条
void SecondScene::progress(Ref* pSender)
{
	//按钮点击音效
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("button.WAV");

	//关掉音乐
	if (myMusicer->isBackgroundMusicPlaying()){
		myMusicer->stopBackgroundMusic();
	}

	//完善
	auto menu1 = getChildByName("menu1");
	auto label = getChildByName("label");
	auto label2 = getChildByName("label2");
	auto map1 = getChildByName("map1");
	auto button = getChildByName("button");
	auto button1 = getChildByName("button1");
	auto star1 = getChildByName("star1");
	auto star2 = getChildByName("star2");
	auto land = getChildByName("land");
	auto label1 = getChildByName("label1");
	removeChild(menu1);
	removeChild(label);
	removeChild(label2);
	removeChild(map1);
	removeChild(button);
	removeChild(button1);
	removeChild(star1);
	removeChild(star2);
	removeChild(land);
	removeChild(label1);

	auto ui = TMXTiledMap::create("ui.tmx");
	addChild(ui, 0, "ui");

	//显示主页
	auto dict3 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn3 = ((String*)dict3->objectForKey("home"))->getCString();
	auto label3 = Label::createWithTTF(chn3, "FeiGunGun.ttf", 80);
	label3->setPosition(480, 450);
	addChild(label3);

	//加载标签
	auto loadLabel = Label::create("Loading...", "Arial", 30);
	loadLabel->setPosition(480, 300);
	addChild(loadLabel);

	//进度框
	auto kuang = Sprite::create("loadKuang.png");
	kuang->setPosition(Vec2(480, 200));
	addChild(kuang);

	//进度条
	auto progress2 = ProgressFromTo::create(5, 0, 100);     //5秒从0%到100%
	auto progTime2 = ProgressTimer::create(Sprite::create("loadBar.png"));
	progTime2->setPosition(Vec2(484, 200));
	this->addChild(progTime2);

	//条形
	progTime2->setType(ProgressTimer::Type::BAR);
	progTime2->setBarChangeRate(Vec2(1, 0));      //设置进度条为横向移动，纵向移动：Vec2（0，1）
	progTime2->setMidpoint(Vec2(0, 0));        //设置进度条从左往右移动，从右往左移动：Vec2（1，0）
	progTime2->runAction(progress2);

	this->scheduleOnce(schedule_selector(SecondScene::home), 5.0f);
}

//失败进度条
void SecondScene::progress1(Ref* pSender)
{
	//按钮点击音效
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("button.WAV");

	//关掉音乐
	if (myMusicer->isBackgroundMusicPlaying()){
		myMusicer->stopBackgroundMusic();
	}

	//完善
	auto menu1 = getChildByName("menu1");
	auto label = getChildByName("label");
	auto label2 = getChildByName("label2");
	auto map1 = getChildByName("map1");
	auto button = getChildByName("button");
	auto button1 = getChildByName("button1");
	auto star1 = getChildByName("star1");
	auto star2 = getChildByName("star2");
	auto land = getChildByName("land");
	auto label0 = getChildByName("label0");
	removeChild(menu1);
	removeChild(label);
	removeChild(label2);
	removeChild(map1);
	removeChild(button);
	removeChild(button1);
	removeChild(star1);
	removeChild(star2);
	removeChild(land);
	removeChild(label0);

	auto ui = TMXTiledMap::create("ui.tmx");
	addChild(ui, 0, "ui");

	//显示第二关
	auto dict3 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn3 = ((String*)dict3->objectForKey("second"))->getCString();
	auto label3 = Label::createWithTTF(chn3, "FeiGunGun.ttf", 80);
	label3->setPosition(480, 450);
	addChild(label3);

	//加载标签
	auto loadLabel = Label::create("Loading...", "Arial", 30);
	loadLabel->setPosition(480, 300);
	addChild(loadLabel);

	//进度框
	auto kuang = Sprite::create("loadKuang.png");
	kuang->setPosition(Vec2(480, 200));
	addChild(kuang);

	//进度条
	auto progress2 = ProgressFromTo::create(5, 0, 100);     //5秒从0%到100%
	auto progTime2 = ProgressTimer::create(Sprite::create("loadBar.png"));
	progTime2->setPosition(Vec2(484, 200));
	this->addChild(progTime2);

	//条形
	progTime2->setType(ProgressTimer::Type::BAR);
	progTime2->setBarChangeRate(Vec2(1, 0));      //设置进度条为横向移动，纵向移动：Vec2（0，1）
	progTime2->setMidpoint(Vec2(0, 0));        //设置进度条从左往右移动，从右往左移动：Vec2（1，0）
	progTime2->runAction(progress2);

	//提示
	auto dict4 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn4 = ((String*)dict4->objectForKey("tips"))->getCString();
	auto label4 = Label::createWithTTF(chn4, "FeiGunGun.ttf", 25);
	label4->setPosition(480, 170);
	addChild(label4);

	this->scheduleOnce(schedule_selector(SecondScene::resume), 5.0f);
}


//菜单 - 2. resume功能
void SecondScene::resume(float)
{
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(1, SecondScene::createScene()));
}

//菜单 - 2. home功能
void SecondScene::home(float)
{
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(1, HelloWorld::createScene()));
}

//菜单 - 2. exit功能
void SecondScene::exit(Ref* pSender)
{
	Director::getInstance()->end();
}