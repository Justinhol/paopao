#include "MyScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "HelloWorldScene.h"
#include "SecondScene.h"


USING_NS_CC;

using namespace cocostudio::timeline;

Scene* MyScene::createScene()
{
	//��������
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
	//Ԥ����������Ч
	myMusicer = CocosDenshion::SimpleAudioEngine::getInstance();
	myMusicer->preloadBackgroundMusic("bgMusic.mp3");
	myMusicer->preloadBackgroundMusic("button.WAV");
	myMusicer->preloadBackgroundMusic("ready.mp3");
	myMusicer->preloadBackgroundMusic("boom.wav");
	myMusicer->preloadBackgroundMusic("round1.mp3");
	myMusicer->preloadBackgroundMusic("dead.mp3");
	myMusicer->preloadBackgroundMusic("pass.mp3");

	//��Ƭ��ͼ
	auto map = TMXTiledMap::create("map.tmx");
	addChild(map, 0, "map");

	//�������� --����
	auto player = Sprite::create("player.png");
	player->setTag(1);
	player->setPosition(20, 580);
	addChild(player, 0, "player");
	auto playerIn = MoveTo::create(2, Vec2(50, 580));//���ݳ���
	auto blink = Blink::create(3.0, 5);//3������˸5��
	auto seq0 = Sequence::create(playerIn, blink, NULL);//��˳��ִ��
	player->runAction(seq0);//������

	//��ť
	auto button = Button::create("button.png");
	button->setScale(0.5);
	button->setPosition(ccp(808, 150));
	this->addChild(button,0,"button");

	//������ǩ
	auto label = Label::createWithTTF("Round One", "FeiGunGun.ttf", 40);
	auto ls = label->getContentSize();
	label->setPosition(-ls.width / 2, 320);
	addChild(label);
	
	//��ǩ�ƶ�
	auto moveIn = MoveTo::create(2.0, Vec2(480, 320));//�ƶ���ǩ������
	auto moveOut = MoveTo::create(2.0, Vec2(1200, 320));//�Ƴ�
	auto delay = DelayTime::create(1.0);//ͣ��1��
	auto seq = Sequence::create(moveIn, delay, moveOut, NULL);//��˳��ִ��
	label->runAction(seq);//�󶨱�ǩ
	this->scheduleOnce(schedule_selector(MyScene::RoundOne), 0.8f);//round1��Ч��ʱ��
	this->scheduleOnce(schedule_selector(MyScene::Ready), 2.8f);//ready��Ч��ʱ��
	this->scheduleOnce(schedule_selector(MyScene::Start), 5.0f);//�������ֶ�ʱ��

	scheduleUpdate();

	return true;
}

//��ʱ�� - 1. �ƶ���ʱ�� - �ϰ����⺯��
bool MyScene::IsCanReach(float x, float y)
{
	auto map = (TMXTiledMap*)getChildByName("map");
	auto mask = map->layerNamed("mask");//��ȡ�ϰ���ͼ��
	auto TileSize = 16;
	int mapX = x / TileSize;
	int col = map->getMapSize().height;
	int mapY = col - y / 16;
	if (mask->getTileAt(Vec2(mapX, mapY)))
		return true;
	return false;
}

//��ʱ�� - 1. �ƶ���ʱ�� - ���ؼ�⺯��
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

//����round1��Ч
void MyScene::RoundOne(float){
	myMusicer->playBackgroundMusic("round1.mp3");
}

//����ready��Ч
void MyScene::Ready(float){
	myMusicer->playBackgroundMusic("ready.mp3");
}

//��Ϸ��ʼ
void MyScene::Start(float){
	//���ű�������
	myMusicer->playBackgroundMusic("bgMusic.mp3");

	//���ݸ���
	auto player = getChildByName("player");
	playerbody = PhysicsBody::createCircle(20);
	playerbody->applyForce(Vec2(500, 0));
	player->setPhysicsBody(playerbody);

	//��ť
	auto button = Button::create("button.png");
	button->addTouchEventListener(CC_CALLBACK_2(MyScene::touchMove, this));
	button->setScale(0.5);
	button->setPressedActionEnabled(true);
	button->setPosition(ccp(808, 150));
	this->addChild(button,0,"button1");

	//��ť������
	auto listener = EventListenerTouchOneByOne::create();
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);	
}


//ִ���ƶ�
void MyScene::toch_Move(float dt)
{
	if (getChildByTag(1))
		playerbody->applyImpulse(Vec2(0, 1000));
}

//������ť�¼�
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

//��ʱ�� - ��ײ���
void MyScene::update(float dt)
{
	auto player = getChildByName("player");

	//�ϰ�����ײ���
	if (IsCanReach(player->getPositionX() + 20, player->getPositionY() + 20) || IsCanReach(player->getPositionX() - 20, player->getPositionY() - 20))
	{
		//������� - ���� - 3. ���Ǳ�ը����
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("boom.wav");

		//������� - 3. ��ը����
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

		//���پ���
		unscheduleUpdate();
		removeChild(player);

		auto dict3 = Dictionary::createWithContentsOfFile("chn.xml");
		auto chn3 = ((String*)dict3->objectForKey("over"))->getCString();
		auto label3 = Label::createWithTTF(chn3, "FeiGunGun.ttf", 40);
		label3->setPosition(480, 400);
		addChild(label3);

		//�����˵�
		ShowMenu();
		return;
	}

	//���ؼ��
	else if (IsCanPass(player->getPositionX() + 7, player->getPositionY() + 7) || IsCanPass(player->getPositionX() - 7, player->getPositionY() - 7))
	{
		//���پ���
		unscheduleUpdate();
		removeChild(player);

		//�����˵�
		ShowMenu1();
		return;
	}
}

//���ٶ���
void MyScene::ExplodeRemove()
{
	removeChildByName("explode");
}

//ʧ�ܲ˵�
void MyScene::ShowMenu()
{
	//������������
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

//���ز˵�
void MyScene::ShowMenu1()
{
	//���Ź�������
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

//���ؽ�����
void MyScene::progress(Ref* pSender)
{
	//��ť�����Ч
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("button.WAV");

	//�ص�����
	if (myMusicer->isBackgroundMusicPlaying()){
		myMusicer->stopBackgroundMusic();
	}

	//����
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

	//��ʾ�ڶ���
	auto dict1 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn1 = ((String*)dict1->objectForKey("second"))->getCString();
	auto label1 = Label::createWithTTF(chn1, "FeiGunGun.ttf", 80);
	label1->setPosition(480, 450);
	addChild(label1);

	//���ر�ǩ
	auto loadLabel = Label::create("Loading...", "Arial", 30);
	loadLabel->setPosition(480, 300);
	addChild(loadLabel);

	//���ȿ�
	auto kuang = Sprite::create("loadKuang.png");
	kuang->setPosition(Vec2(480, 200));
	addChild(kuang);

	//������
	auto progress2 = ProgressFromTo::create(5, 0, 100);     //5���0%��100%
	auto progTime2 = ProgressTimer::create(Sprite::create("loadBar.png"));
	progTime2->setPosition(Vec2(484, 200));
	this->addChild(progTime2);

	//����
	progTime2->setType(ProgressTimer::Type::BAR);
	progTime2->setBarChangeRate(Vec2(1, 0));      //���ý�����Ϊ�����ƶ��������ƶ���Vec2��0��1��
	progTime2->setMidpoint(Vec2(0, 0));        //���ý��������������ƶ������������ƶ���Vec2��1��0��
	progTime2->runAction(progress2);

	//��ʾ
	auto dict4 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn4 = ((String*)dict4->objectForKey("tips"))->getCString();
	auto label4 = Label::createWithTTF(chn4, "FeiGunGun.ttf", 25);
	label4->setPosition(480, 170);
	addChild(label4);

	this->scheduleOnce(schedule_selector(MyScene::next), 5.0f);
}

//ʧ�ܽ�����
void MyScene::progress1(Ref* pSender)
{
	//��ť�����Ч
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("button.WAV");

	//�ص�����
	if (myMusicer->isBackgroundMusicPlaying()){
		myMusicer->stopBackgroundMusic();
	}

	//����
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

	//��ʾ��һ��
	auto dict1 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn1 = ((String*)dict1->objectForKey("first"))->getCString();
	auto label1 = Label::createWithTTF(chn1, "FeiGunGun.ttf", 80);
	label1->setPosition(480, 450);
	addChild(label1);

	//���ر�ǩ
	auto loadLabel = Label::create("Loading...", "Arial", 30);
	loadLabel->setPosition(480, 300);
	addChild(loadLabel);

	//���ȿ�
	auto kuang = Sprite::create("loadKuang.png");
	kuang->setPosition(Vec2(480, 200));
	addChild(kuang);

	//������
	auto progress2 = ProgressFromTo::create(5, 0, 100);     //5���0%��100%
	auto progTime2 = ProgressTimer::create(Sprite::create("loadBar.png"));
	progTime2->setPosition(Vec2(484, 200));
	this->addChild(progTime2);

	//����
	progTime2->setType(ProgressTimer::Type::BAR);
	progTime2->setBarChangeRate(Vec2(1, 0));      //���ý�����Ϊ�����ƶ��������ƶ���Vec2��0��1��
	progTime2->setMidpoint(Vec2(0, 0));        //���ý��������������ƶ������������ƶ���Vec2��1��0��
	progTime2->runAction(progress2);

	//��ʾ
	auto dict3 = Dictionary::createWithContentsOfFile("chn.xml");
	auto chn3 = ((String*)dict3->objectForKey("tips"))->getCString();
	auto label3 = Label::createWithTTF(chn3, "FeiGunGun.ttf", 25);
	label3->setPosition(480, 170);
	addChild(label3);

	this->scheduleOnce(schedule_selector(MyScene::resume), 5.0f);
}

//�˵� - 2. resume����
void MyScene::resume(float)
{
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(1, MyScene::createScene()));
}

//�˵� - 2. next����
void MyScene::next(float)
{
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(1, SecondScene::createScene()));
}

//�˵� - 2. exit����
void MyScene::exit(Ref* pSender)
{
	Director::getInstance()->end();
}