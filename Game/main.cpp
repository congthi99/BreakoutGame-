#include <SFML/Graphics.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include "Menu.h"
using namespace std;
using namespace sf;
int x = 5;
float windowWidth ( 800 ), windowHeight( 600 );
float ballRadius( 10.f ), ballVelocity( 6.f );
float paddleWidth( 100.f ), paddleHeight( 20.f ), paddleVelocity( 10.f );
float blockWidth( 60.f ), blockHeight( 20.f );
float countBlocksX( 11 ), countBlocksY( 8 );
float countBlocks2X(11), countBlocks2Y(3);
bool isPlaying = true;
sf::SoundBuffer ballSoundBuffer;
sf::SoundBuffer ballPlaying;
sf::Sound loseGameSound;
sf::Sound playingSound;
struct Ball
{
        bool run=false;
        Vector2f velocity{ -ballVelocity, -ballVelocity };
        CircleShape shape;
        Ball(float mX, float mY)
        {
                shape.setRadius(ballRadius);
                shape.setPosition(mX, mY);
                shape.setFillColor(Color::White);
                shape.setOrigin(ballRadius, ballRadius);
        }

        void update()
        {
                //Need to make the ball bounce of the window edges
                if(run) shape.move(velocity);

                //If it's leaving on the left edge, we set a positive horizontal value.
                if (left() < 0){
                        velocity.x = ballVelocity;
                        //playingSound.play();
                        }
                //Same for the right
                else if (right() > windowWidth){
                        velocity.x = -ballVelocity;
                        //playingSound.play();
                        }
                //Top
                if (top() < 0){
                        velocity.y = ballVelocity;
                        //playingSound.play();
                        }
                //And bottom

        }

        float x() { return shape.getPosition().x; }
        float y() { return shape.getPosition().y; }
        float left() { return x() - ballRadius; }
        float right() { return x() + ballRadius; }
        float top() { return y() - ballRadius; }
        float bottom() { return y() + ballRadius; }
};

//Create the Rectangle shape class for the brick
struct Rectangle
{
        RectangleShape shape;
        float x()               { return shape.getPosition().x; }
        float y()               { return shape.getPosition().y; }
        float left()    { return x() - shape.getSize().x / 2.f; }
        float right()   { return x() + shape.getSize().x / 2.f; }
        float top()             { return y() - shape.getSize().y / 2.f; }
        float bottom()  { return y() + shape.getSize().y / 2.f; }
};

//Class for the paddle
struct Paddle : public Rectangle
{
        //Create a variable for speed.
        Vector2f velocity;
        //Set the variables for the paddle rectangle shape.
        Paddle(float mX, float mY)
        {
                shape.setPosition(mX, mY);
                shape.setSize({ paddleWidth, paddleHeight });
                shape.setFillColor(Color::Cyan);
                shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
        }
        double getPositionX()
        {
            return shape.getPosition().x;
        }
        // Within the update function we check if the player is moving the paddle
        void update()
        {
                shape.move(velocity);
                //To ensure that the paddle stays inside the window we only change the Velocity when it's inside the boundaries
                //Making it impossible to move outside when the initial velocity is set to zero
                if (Keyboard::isKeyPressed(Keyboard::Key::Left) && left() > 0)
                velocity.x = -paddleVelocity;

                else if (Keyboard::isKeyPressed(Keyboard::Key::Right) && right() < windowWidth)
                        velocity.x = paddleVelocity;
                //If the player isn't pressing a buttom (legt/right) the velocity is set to zero.
                else
                        velocity.x = 0;
        }
        void restart()
        {
            shape.setPosition(windowWidth/2,windowHeight -50 );
        }
};
//Another class for the bricks
struct Brick : public Rectangle
{
        bool destroyed{ false };

        Brick(float mX, float mY)
        {
                shape.setPosition(mX, mY);
                shape.setSize({ blockWidth, blockHeight });
                shape.setFillColor(Color::Black);
                shape.setOrigin(blockWidth / 2.f, blockHeight / 2.f);
        }
        int levelbtrick;
};

//C++ Feature template allows us to create a generic funtion to check if two shapes are intersecting or colliding.
template <class T1, class T2>
bool isIntersecting(T1& mA, T2& mB)
{
        return mA.right() >= mB.left() && mA.left() <= mB.right() &&
                mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
}

void collisionTest(Paddle& mPaddle, Ball& mBall)
{
        if (!isIntersecting(mPaddle, mBall)) return;
        if(mBall.run)playingSound.play();
        mBall.velocity.y = -ballVelocity;

        mBall.velocity.x *=1;
}


void collisionTest(Brick& mBrick, Ball& mBall)
{
        if (!isIntersecting(mBrick, mBall)) return;
        switch(mBrick.levelbtrick)
        {
            case 1: mBall.shape.setFillColor(Color::Green);break;
            case 2: mBall.shape.setFillColor(Color::Red);break;
            case 3: mBall.shape.setFillColor(Color::Yellow);break;
            default: mBall.shape.setFillColor(Color::Blue);break;
        }
        mBrick.levelbtrick--;
        if(mBrick.levelbtrick==0){
        mBrick.destroyed = true;}
        else
        {
            switch(mBrick.levelbtrick)
            {
                case 1: mBrick.shape.setFillColor(Color::Green);break;
                case 2: mBrick.shape.setFillColor(Color::Red);break;
                case 3: mBrick.shape.setFillColor(Color::Yellow);break;
                default: mBrick.shape.setFillColor(Color::Blue);break;
            }
        }
        playingSound.play();
        float overlapLeft{ mBall.right() - mBrick.left() };
        float overlapRight{ mBrick.right() - mBall.left() };
        float overlapTop{ mBall.bottom() - mBrick.top() };
        float overlapBottom{ mBrick.bottom() - mBall.top() };

        bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
        bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

        float minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
        float minOverlapY{ ballFromTop ? overlapTop : overlapBottom };

        if (abs(minOverlapX) < abs(minOverlapY))
                mBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
        else
                mBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;
}
int main()
{

        //We render/create the window
        RenderWindow window(VideoMode(windowWidth, windowHeight ), "Breakout Game" ,Style::Titlebar);
        window.setFramerateLimit(60);
        Paddle paddle{ windowWidth / 2, windowHeight - 50 };
        ballSoundBuffer.loadFromFile("loseSound.wav");

                //return EXIT_FAILURE;
        loseGameSound.setBuffer(ballSoundBuffer);
        ballPlaying.loadFromFile("Pickup_Coin5.wav");
        playingSound.setBuffer(ballPlaying);
        // Load the text font
        sf::Font font;
        if (!font.loadFromFile("blackrose.ttf"))
                return EXIT_FAILURE;

        // Initialize the pause message
        sf::Text loseGame;
        loseGame.setFont(font);
        loseGame.setCharacterSize(100);
        loseGame.setPosition(120.f, 200.f);
        loseGame.setFillColor(Color::White);
        loseGame.setString("Game Over !!!");

        // Load the sounds used in the game


        //int x = 5;
//Here we use an unconditiional goto statement to allow the user to restart the game.
        Text startGame;
        startGame.setFont(font);
        startGame.setCharacterSize(60);
        startGame.setPosition(150.f,windowHeight/2);
        startGame.setFillColor(Color::White);
        startGame.setString("Press Space to start !!");

        string s;
        Text leveltext;
        leveltext.setFont(font);
        leveltext.setCharacterSize(20);
        leveltext.setPosition(windowWidth-100,10);
        leveltext.setFillColor(Color::White);


restart:
        int level=1;
        s=level+48;
        leveltext.setString("Level : "+s);
        //We reference the Ball, Paddle and Bricks
        Ball ball{ windowWidth / 2, windowHeight -70 };
        paddle.restart();
        vector<Brick> bricks;
        //vector<Brick2> bricks2;

        for (int iX{ 0 }; iX < countBlocksX; ++iX)
                for (int iY{ 0 }; iY < countBlocksY; ++iY){
                    bricks.emplace_back(
                                (iX + 1) * (blockWidth + 3) + 22, (iY + 2) * (blockHeight + 3));
                    int thutu=rand()%4;
                    switch(thutu)
                    {
                        case 0: bricks[bricks.size()-1].shape.setFillColor(Color::Green),bricks[bricks.size()-1].levelbtrick=1;break;
                        case 1: bricks[bricks.size()-1].shape.setFillColor(Color::Red),bricks[bricks.size()-1].levelbtrick=2;break;
                        case 2: bricks[bricks.size()-1].shape.setFillColor(Color::Yellow),bricks[bricks.size()-1].levelbtrick=3;break;
                        default: bricks[bricks.size()-1].shape.setFillColor(Color::Blue),bricks[bricks.size()-1].levelbtrick=4;break;
                    }
                }





        while (true)
        {
                window.clear(Color::Black);
                if (Keyboard::isKeyPressed(Keyboard::Key::Space)){ball.run=true,isPlaying=true;}

                if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
                        break;
                ball.update();
                if(ball.bottom()> windowHeight) {isPlaying=false;ballVelocity=6.f;}
                paddle.update();
                if(!ball.run)ball.shape.setPosition(paddle.getPositionX(),ball.shape.getPosition().y);
                collisionTest(paddle, ball);
                for (auto& brick : bricks) collisionTest(brick, ball);
                bricks.erase(remove_if(begin(bricks), end(bricks),
                        [](const Brick& mBrick)
                {
                        return mBrick.destroyed;
                }),
                        end(bricks));
            if(bricks.empty()){
                level++;
                s=level+48;
                leveltext.setString("Level : "+s);
                for (int iX{ 0 }; iX < rand()%9+4; ++iX)
                for (int iY{ 0 }; iY < rand()%9+4; ++iY){
                    bricks.emplace_back(
                                (iX + 1) * (blockWidth + 3) + 22, (iY + 2) * (blockHeight + 3));
                    int thutu=rand()%4;
                    switch(thutu)
                    {
                        case 0: bricks[bricks.size()-1].shape.setFillColor(Color::Green),bricks[bricks.size()-1].levelbtrick=1;break;
                        case 1: bricks[bricks.size()-1].shape.setFillColor(Color::Red),bricks[bricks.size()-1].levelbtrick=2;break;
                        case 2: bricks[bricks.size()-1].shape.setFillColor(Color::Yellow),bricks[bricks.size()-1].levelbtrick=3;break;
                        default: bricks[bricks.size()-1].shape.setFillColor(Color::Blue),bricks[bricks.size()-1].levelbtrick=4;break;
                    }
                }
                ball.run=false;
                ball.shape.setPosition(windowWidth/2,windowHeight-70);
                paddle.restart();
                ballVelocity*=1.5;
                }
                if (isPlaying)
                {
                        window.draw(ball.shape);
                        window.draw(paddle.shape);
                        window.draw(leveltext);
                        if(!ball.run)window.draw(startGame);
                        for (auto& brick : bricks) window.draw(brick.shape);
                }

                else
                {
                        window.clear(Color::Black);
                        // Draw the pause message
                        loseGameSound.play();
                        window.draw(loseGame);
                        if (Keyboard::isKeyPressed(Keyboard::Key::Space))
                                goto restart;

                }
                window.display();


        }

        return 0;
}

