#include "MyScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "HelloWorldScene.h"
#include "SecondScene.h"


USING_NS_CC;

using namespace cocostudio::timeline;

Scene* MyScene::createScene()
{
	//物理引擎
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2(0, -300));
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
	scene->getPhysicsWorld()->setSubsteps(10);

	auto layer = MyScene::create();

	scene->addChild(layer);

	return scene;
}

bool MyScene::init()
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
	myMusicer->preloadBackgroundMusic("round1.mp3");
	myMusicer->preloadBackgroundMusic("dead.mp3");
	myMusicer->preloadBackgroundMusic("pass.mp3");

	//瓦片地图
	auto map = TMXTiledMap::create("map.tmx");
	addChild(map, 0, "map");

	//创建精灵 --泡泡
	auto player = Sprite::create("player.png");
	player->setTag(1);
	player->setPosition(20, 580);
	addChild(player, 0, "player");
	auto playerIn = MoveTo::create(2, Vec2(50, 580));//泡泡出场
	auto blink = Blink::create(3.0, 5);//3秒内闪烁5次
	auto seq0 = Sequence::create(playerIn, blink, NULL);//按顺序执行
	player->runAction(seq0);//绑定泡泡

	//按钮
	auto button = Button::create("button.png");
	button->setScale(0.5);
	button->setPosition(ccp(808, 150));
	this->addChild(button,0,"button");

	//创建标签
	auto label = Label::createWithTTF("Round One", "FeiGunGun.ttf", 40);
	auto ls = label->getContentSize();
	label->setPosition(-ls.width / 2, 320);
	addChild(label);
	
	//标签移动
	auto moveIn = MoveTo::create(2.0, Vec2(480, 320));//移动标签至中心
	auto moveOut = MoveTo::create(2.0, Vec2(1200, 320));//移出
	auto delay = DelayTime::create(1.0);//停留1秒
	auto seq = Sequence::create(moveIn, delay, moveOut, NULL);//按顺序执行
	label->runAction(seq);//绑定标签
	this->scheduleOnce(schedule_selector(MyScene::RoundOne), 0.8f);//round1音效定时器
	this->scheduleOnce(schedule_selector(MyScene::Ready), 2.8f);//ready音效定时器
	this->scheduleOnce(schedule_selector(MyScene::Start), 5.0f);//背景音乐定时器

	scheduleUpdate();

	return true;
}

//定时器 - 1. 移动定时器 - 障碍物检测函数
bool MyScene::IsCanReach(float x, float y)
{
	auto map = (TMXTiledMap*)getChildByName("map");
	auto mask = map->layerNamed("mask");//获取障碍物图层
	auto TileSize = 16;
	int mapX = x / TileSize;
	int col = map->getMapSize().height;
	int mapY = col - y / 16;
	if (mask->getTileAt(Vec2(mapX, mapY)))
		return true;
	return false;
}

//定时器 - 1. 移动定时器 - 过关检测函数
bool MyScene::IsCanPass(float x, float y)
{
	auto map = (TMXTiledMap*)getChildByName("map");
	auto pass = map->layerNamed("pass");
	auto TileSize = 16;
	int mapX = x / TileSize;
	int col = map->getMapSize().height;
	int mapY = col - y / 16;
	if (pass->getTileAt(Vec2(mapX, mapY)))
		return true;
	return false;
}

//播放round1音效
void MyScene::RoundOne(float){
	myMusicer->playBackgroundMusic("round1.mp3");
}

//播放ready音效
void MyScene::Ready(float){
	myMusicer->playBackgroundMusic("ready.mp3");
}

//游戏开始
void MyScene::Start(float){
	//播放背景音乐
	myMusicer->playBackgroundMusic("bgMusic.mp3");

	//泡泡刚体
	auto player = getChildByName("player");
	playerbody = PhysicsBody::createCircle(20);
	playerbody->applyForce(Vec2(500, 0));
	player->setPhysicsBody(playerbody);

	//按钮
	auto button = Button::create("button.png");
	button->addTouchEventListener(CC_CALLBACK_2(MyScene::touchMove, this));
	button->setScale(0.5);
	button->setPressedActionEnabled(true);
	button->setPosition(ccp(808, 150));
	this->addChild(button,0,"button1");

	//按钮监听器
	auto listener = EventListenerTouchOneByOne::create();
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);	
}


//执行移动
void MyScene::toch_Move(float dt)
{
	if (getChildByTag(1))
		playerbody->applyImpulse(Vec2(0, 1000));
}

//长按按钮事件
void MyScene::touchMove(cocos2d::Ref* pSender, Widget::TouchEventType event)
{
	if (event == Widget::TouchEventType::BEGAN)
	{
		this->schedule(schedule_selector(MyScene::toch_Move), 0.0f);
	}

	if (event == Widget::TouchEventType::ENDED)
	{
		this->unschedule(schedule_selector(MyScene::toch_Move));
	}
}

//定时器 - 碰撞检测
void MyScene::update(float dt)
{
	auto player = getChildByName("player");

	//障碍物碰撞检测
	if (IsCanReach(player->getPositionX() + 20, player->getPositionY() + 20) || IsCanReach(player->getPositionX() - 20, player->getPositionY() - 20))
	{
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
		auto removeExplode = CallFunc::create(CC_CALLBACK_0(MyScene::ExplodeRemove, this));
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
		//销毁精灵
		unscheduleUpdate();
		removeChild(player);

		//弹出菜单
		ShowMenu1();
		return;
	}
}

//销毁动画
void MyScene::ExplodeRemove()
{
	removeChildByName("explode");
}

//失败菜单
void MyScene::ShowMenu()
{
	//播放死亡音乐
	myMusicer->playBackgroundMusic("dead.mp3");

	if (!getChildByName("menu"))
	{
		auto resumeLabel = Label::create("Resume", "Arial", 30);
		auto resumeMI = MenuItemLabel::create(resumeLabel, this, menu_selector(MyScene::progress1));
		resumeMI->setPosition(-100, 0);
		auto exitLabel = Label::create("Exit", "Arial", 30);
		auto exitMI = MenuItemLabel::create(exitLabel, this, menu_selector(MyScene::exit));
		exitMI->setPosition(100, 0);
		auto menu = Menu::create(resumeMI, exitMI, NULL);
		addChild(menu, 0, "menu");
	}
	else
		getChildByName("menu")->setVisible(true);
}

//过关菜单
void MyScene::ShowMenu1()
{
	//播放过关音乐
	myMusicer->playBackgroundMusic("pass.mp3");


	auto dict2 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn2 = ((String*)dict2->objectForKey("pass"))->getCString();
	auto label2 = Label::createWithTTF(chn2, "FeiGunGun.ttf", 40);
	label2->setPosition(480, 430);
	addChild(label2,0,"label2");

	if (!getChildByName("menu1"))
	{
		auto nextLabel = Label::create("Next", "Arial", 30);
		auto nextMI = MenuItemLabel::create(nextLabel, this, menu_selector(MyScene::progress));
		nextMI->setPosition(0, 0);
		auto menu1 = Menu::create(nextMI,NULL);
		addChild(menu1, 0, "menu1");
	}
	else
		getChildByName("menu1")->setVisible(true);
}

//过关进度条
void MyScene::progress(Ref* pSender)
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
	auto map = getChildByName("map");
	auto button = getChildByName("button");
	auto button1 = getChildByName("button1");
	removeChild(menu1);
	removeChild(label);
	removeChild(label2);
	removeChild(map);
	removeChild(button);
	removeChild(button1);

	auto ui = TMXTiledMap::create("ui.tmx");
	addChild(ui, 0, "ui");

	//显示第二关
	auto dict1 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn1 = ((String*)dict1->objectForKey("second"))->getCString();
	auto label1 = Label::createWithTTF(chn1, "FeiGunGun.ttf", 80);
	label1->setPosition(480, 450);
	addChild(label1);

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

	this->scheduleOnce(schedule_selector(MyScene::next), 5.0f);
}

//失败进度条
void MyScene::progress1(Ref* pSender)
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
	auto map = getChildByName("map");
	auto button = getChildByName("button");
	auto button1 = getChildByName("button1");
	removeChild(menu1);
	removeChild(label);
	removeChild(label2);
	removeChild(map);
	removeChild(button);
	removeChild(button1);

	auto ui = TMXTiledMap::create("ui.tmx");
	addChild(ui, 0, "ui");

	//显示第一关
	auto dict1 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn1 = ((String*)dict1->objectForKey("first"))->getCString();
	auto label1 = Label::createWithTTF(chn1, "FeiGunGun.ttf", 80);
	label1->setPosition(480, 450);
	addChild(label1);

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
	auto dict3 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn3 = ((String*)dict3->objectForKey("tips"))->getCString();
	auto label3 = Label::createWithTTF(chn3, "FeiGunGun.ttf", 25);
	label3->setPosition(480, 170);
	addChild(label3);

	this->scheduleOnce(schedule_selector(MyScene::resume), 5.0f);
}

//菜单 - 2. resume功能
void MyScene::resume(float)
{
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(1, MyScene::createScene()));
}

//菜单 - 2. next功能
void MyScene::next(float)
{
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(1, SecondScene::createScene()));
}

//菜单 - 2. exit功能
void MyScene::exit(Ref* pSender)
{
	Director::getInstance()->end();
}