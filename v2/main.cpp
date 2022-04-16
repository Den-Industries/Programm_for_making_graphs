#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <Windows.h>

using namespace sf;
using namespace std;

float Long(float f, float s)
{
	return sqrt(f * f + s * s);
}

struct Path
{
	vector<int> vershini;
	float pathvall = 0;
	Path() {}
	Path(float PathVall) 
	{
		pathvall = PathVall;
	}
};

struct Rebro
{
	int first, second;
	float vall, himsled = 0;
	bool highlight = false;
	Rebro(int First, int Second, float Vall = 1.0)
	{
		first = First; second = Second; vall = Vall;
	}
};

int ScreenSize[2], choosens[2], choosenrebro = -1, timer = 0, QofMurashki;
bool choosenb = true, debug = false, murashki = false;
RenderWindow window;
vector<CircleShape> vershini;
vector<Rebro> rebra;
Path FindedPath;
Image bg;
Color bgclrs[4] = {
	Color(rand() % 64 + 64, rand() % 64 + 64, rand() % 64 + 64),
Color(rand() % 64 + 64, rand() % 64 + 64, rand() % 64 + 64),
Color(rand() % 64 + 64, rand() % 64 + 64, rand() % 64 + 64),
Color(rand() % 64 + 64, rand() % 64 + 64, rand() % 64 + 64) };
Texture txtrbg;
Sprite bgspr;

struct Murashka
{
	int kuda = 0, otkuda = 0, place = 0, placerebro = 0;
	float nudolgoesho = 0;
	bool moving = false;
	vector<int> gdeyabil;
	Murashka(int Kuda, int Otkuda)
	{
		kuda = Kuda; otkuda = Otkuda; place = Otkuda;
	}
	void Move(float TimesGone)
	{
		if (moving)
		{
			nudolgoesho += TimesGone * 0.0005;
			if (nudolgoesho > rebra[placerebro].vall)
			{
				nudolgoesho = 0; moving = false;
				place = rebra[placerebro].first + rebra[placerebro].second - place;
				if (place == kuda)
				{
					kuda += otkuda; otkuda = kuda - otkuda; kuda -= otkuda;
					for (int i = 0; i < gdeyabil.size(); i++)
					{
						rebra[gdeyabil[i]].himsled += 0.5;
						if (rebra[gdeyabil[i]].himsled > 20)
							rebra[gdeyabil[i]].himsled = 20;
					}
					gdeyabil.clear();
					for (int i = 0; i < rebra.size(); i++)
					{
						rebra[i].himsled -= 0.2;
						if (rebra[i].himsled < 0) rebra[i].himsled = 0;
					}
				}
			}
		}
		else
		{
			vector< pair<float, int> > kudayamogypoiti;
			for (int i = 0; i < rebra.size(); i++)
				if (rebra[i].first == place || rebra[i].second == place)
				{
					bool mogu = true;
					for (int u = 0; u < gdeyabil.size(); u++)
						if (i == gdeyabil[u])
							mogu = false;
					if (mogu)
					{
						kudayamogypoiti.push_back({ rebra[i].himsled, i });
					}
				}
			if (kudayamogypoiti.size() > 0)
			{
				float allhimsled = 0;
				for (int i = 0; i < kudayamogypoiti.size(); i++)
				{
					kudayamogypoiti[i].first += 1.0;
					allhimsled += kudayamogypoiti[i].first;
				}
				float kudaidem = float(rand() % int(allhimsled * 10)) * 0.1f;
				int kogo = 0;
				while (kudaidem > kudayamogypoiti[kogo].first)
				{
					kudaidem -= kudayamogypoiti[kogo].first; kogo += 1;
				}
				gdeyabil.push_back(kudayamogypoiti[kogo].second);
				placerebro = kudayamogypoiti[kogo].second;
				moving = true;
			}
			else
			{
				place = otkuda; gdeyabil.clear();
			}
		}
	}
};

vector<Murashka> muravi;

Path FindPath(int First, int Second, Path PartOfPath = Path(), int inseption = 0)
{
	if (debug) cout << "INSEPTION " << inseption << endl;
	PartOfPath.vershini.push_back(First);
	if (First == Second)
	{
		return PartOfPath;
	}
	else
	{
		if (debug) cout << "INSEPTION " << inseption << "   TIMER " << timer << endl; timer++;
		if (debug) for (int i = 0; i < PartOfPath.vershini.size(); i++)
			cout << PartOfPath.vershini[i] << endl;
		if (debug) cout << "\t1" << endl << endl;

		vector<Path> Pathes;
		for (int i = 0; i < rebra.size(); i++)
			if (rebra[i].first == First || rebra[i].second == First)
			{
				bool been = false;
				for (int u = 0; u < PartOfPath.vershini.size(); u++)
					if ((PartOfPath.vershini[u] == rebra[i].first && rebra[i].second == First) ||
						(PartOfPath.vershini[u] == rebra[i].second && rebra[i].first == First)) been = true;

				if (!been)
				{
					if (rebra[i].second == First)
						Pathes.push_back(FindPath(rebra[i].first, Second, PartOfPath, inseption + 1));
					else
						Pathes.push_back(FindPath(rebra[i].second, Second, PartOfPath, inseption + 1));
					Pathes[Pathes.size() - 1].pathvall += rebra[i].vall;
					if (debug) cout << "INSEPTION " << inseption << "   TIMER " << timer << endl; timer++;
					if (debug) for (int i = 0; i < Pathes[Pathes.size() - 1].vershini.size(); i++)
						cout << Pathes[Pathes.size() - 1].vershini[i] << endl;
					if (debug) cout << "\t5 Val " << Pathes[Pathes.size() - 1].pathvall << endl << endl << endl;
				}
			}
		if (debug) cout << "INSEPTION " << inseption << "   TIMER " << timer << endl; timer++;
		if (debug) for (int i = 0; i < PartOfPath.vershini.size(); i++)
			cout << PartOfPath.vershini[i] << endl;
		if (debug) cout << "\t2" << endl << endl;
		float min = 9999999.0; int minnum = -1;
		for (int i = 0; i < Pathes.size(); i++)
		{
			if (Pathes[i].pathvall < min)
			{
				min = Pathes[i].pathvall; minnum = i;
			}
		}
		if (min > 8999999.0)
		{
			return Path(9999999.0);
		}
		else
		{
			for (int i = 1; i < Pathes[minnum].vershini.size(); i++)
				PartOfPath.vershini.push_back(Pathes[minnum].vershini[i]);
			PartOfPath.pathvall += Pathes[minnum].pathvall;
			if (debug) cout << "INSEPTION " << inseption << "   TIMER " << timer << endl; timer++;
			if (debug) for (int i = 0; i < PartOfPath.vershini.size(); i++)
				cout << PartOfPath.vershini[i] << endl;
			if (debug) cout << "\t3" << endl << endl << endl;
			return PartOfPath;
		}
	}
}

void GetDesktopResolution()
{
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
	ScreenSize[0] = GetSystemMetrics(SM_CXSCREEN);
	ScreenSize[1] = GetSystemMetrics(SM_CYSCREEN);
}

int main()
{
	srand(time(NULL)); GetDesktopResolution(); setlocale(LC_ALL, "ru");
	window.create(VideoMode(ScreenSize[0], ScreenSize[1]), "Programm", Style::Fullscreen, ContextSettings(0, 0, 8));
	Font font;
	font.loadFromFile("font.ttf");
	Text text("", font, 48);
	text.setStyle(sf::Text::Bold);
	text.setOutlineThickness(3);
	text.setFillColor(Color(0, 120, 120));
	text.setOutlineColor(Color(120, 0, 120));
	bg.create(ScreenSize[0], ScreenSize[1]);
	for (int x = 0; x < ScreenSize[0]; x++)
		for (int y = 0; y < ScreenSize[1]; y++)
		{
			bg.setPixel(x, y,
				Color(
					abs((int(bgclrs[0].r * (Long(x, y) / Long(ScreenSize[0], ScreenSize[1])) 
					+ bgclrs[1].r * (Long(ScreenSize[0] - x, y) / Long(ScreenSize[0], ScreenSize[1]))
					+ bgclrs[2].r * (Long(x, ScreenSize[1] - y) / Long(ScreenSize[0], ScreenSize[1]))
					+ bgclrs[3].r * (Long(ScreenSize[0] - x, ScreenSize[1] - y) / Long(ScreenSize[0], ScreenSize[1]))) % 510) - 255),
					abs((int(bgclrs[0].g * (Long(x, y) / Long(ScreenSize[0], ScreenSize[1]))
					+ bgclrs[1].g * (Long(ScreenSize[0] - x, y) / Long(ScreenSize[0], ScreenSize[1]))
					+ bgclrs[2].g * (Long(x, ScreenSize[1] - y) / Long(ScreenSize[0], ScreenSize[1]))
					+ bgclrs[3].g * (Long(ScreenSize[0] - x, ScreenSize[1] - y) / Long(ScreenSize[0], ScreenSize[1]))) % 510) - 255),
					abs((int(bgclrs[0].b * (Long(x, y) / Long(ScreenSize[0], ScreenSize[1]))
					+ bgclrs[1].b * (Long(ScreenSize[0] - x, y) / Long(ScreenSize[0], ScreenSize[1]))
					+ bgclrs[2].b * (Long(x, ScreenSize[1] - y) / Long(ScreenSize[0], ScreenSize[1]))
					+ bgclrs[3].b * (Long(ScreenSize[0] - x, ScreenSize[1] - y) / Long(ScreenSize[0], ScreenSize[1]))) % 510) - 255)
				));
		}
	txtrbg.loadFromImage(bg);
	bgspr.setTexture(txtrbg);
	Clock clk;
	float alltime = 0, mousecd = 300;
	bool press = false, pathhh = false;
	while (window.isOpen())
	{
		float TimesGone = float(clk.getElapsedTime().asMicroseconds()) * 0.001f; 
		clk.restart(); alltime += TimesGone * 2;
		if (mousecd < 300) mousecd += TimesGone;
		Vector2f pos = window.mapPixelToCoords(Mouse::getPosition(window)); 
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed) window.close();
			if (window.hasFocus()) if ((event.type == sf::Event::MouseButtonPressed))
			{
				if (event.mouseButton.button == sf::Mouse::Right) 
				{
					mousecd = 0;
					bool find = false;
					for (int i = 0; i < vershini.size(); i++)
						if (vershini[i].getGlobalBounds().contains(pos))
						{
							for (int u = 0; u < rebra.size(); u++)
							{
								if (rebra[u].first == i || rebra[u].second == i)
									rebra.erase(rebra.begin() + u);
							}
							for (int u = 0; u < rebra.size(); u++)
							{
								if (rebra[u].first > i) rebra[u].first -= 1;
								if (rebra[u].second > i) rebra[u].second -= 1;
							}
							vershini.erase(vershini.begin() + i);
							find = true;
						}
					if (!find)
					{
						vershini.push_back(CircleShape());
						vershini[vershini.size() - 1].setFillColor(Color(rand() % 64 + 64, 196, rand() % 128 + 64));
						vershini[vershini.size() - 1].setPointCount(8);
						vershini[vershini.size() - 1].setOutlineThickness(5);
						vershini[vershini.size() - 1].setRadius(20);
						vershini[vershini.size() - 1].setPosition(pos.x - vershini[vershini.size() - 1].getGlobalBounds().width / 2, 
							pos.y - vershini[vershini.size() - 1].getGlobalBounds().height / 2);
						choosens[1 - choosenb] = vershini.size() - 1;
						choosenb = !choosenb;
					}
				}
			}
			if (window.hasFocus()) if (event.type == sf::Event::MouseWheelScrolled)
			{
				if (Keyboard::isKeyPressed(Keyboard::LControl))
					rebra[choosenrebro].vall += event.mouseWheelScroll.delta * 0.1;
				else
					rebra[choosenrebro].vall += event.mouseWheelScroll.delta * 0.01;
				if (rebra[choosenrebro].vall < 0.1) rebra[choosenrebro].vall = 0.1;
				if (rebra[choosenrebro].vall > 10.0) rebra[choosenrebro].vall = 10.0;
			}
		}
		window.clear();
		window.draw(bgspr);
		for (int i = 0; i < rebra.size(); i++)
		{
			bool highlight = false;
			if (FindedPath.vershini.size() > 0)
			for (int u = 0; u < FindedPath.vershini.size() - 1; u++)
			{
				if ((FindedPath.vershini[u] == rebra[i].first &&
					FindedPath.vershini[u + 1] == rebra[i].second) ||
					FindedPath.vershini[u] == rebra[i].second &&
					FindedPath.vershini[u + 1] == rebra[i].first) highlight = true;
			}
			int dist = Long(vershini[rebra[i].first].getPosition().x - vershini[rebra[i].second].getPosition().x,
				vershini[rebra[i].first].getPosition().y - vershini[rebra[i].second].getPosition().y);
			float xx = (vershini[rebra[i].first].getPosition().x - vershini[rebra[i].second].getPosition().x) / dist,
				yy = (vershini[rebra[i].first].getPosition().y - vershini[rebra[i].second].getPosition().y) / dist;
			float radius = rebra[i].vall * 4;
			if (radius < 2) radius = 2;
			if (radius > 16) radius = 16;
			CircleShape line(radius);
			line.setPointCount(12);
			line.setOrigin(line.getGlobalBounds().width / 2, line.getGlobalBounds().height / 2);
			for (int u = 18; u < dist; u += radius * 1.5)
			{
				if (highlight)
				{
					CircleShape need(20);
					need.setOrigin(20, 20);
					need.setFillColor(Color(96 * (choosenrebro != i) + 32 + abs(min((int(alltime * 0.1 + u + i * 100) % 762), 254) - 127.0),
						96 * (choosenrebro != i) + 32 + abs(min(max((int(alltime * 0.1 + u + i * 100) % 762) - 254.0, 0.0), 254.0) - 127.0),
						96 * (choosenrebro != i) + 32 + abs(min(max((int(alltime * 0.1 + u + i * 100) % 762) - 508.0, 0.0), 254.0) - 127.0), 24));
					need.setPosition(vershini[rebra[i].first].getPosition().x + (vershini[rebra[i].first].getGlobalBounds().width * 0.5) - u * xx,
						vershini[rebra[i].first].getPosition().y + (vershini[rebra[i].first].getGlobalBounds().height * 0.5) - u * yy);
					window.draw(need);
				}
				if (muravi.size() > 0 && rebra[i].himsled > 0)
				{
					CircleShape need(radius + rebra[i].himsled);
					need.setOrigin(need.getGlobalBounds().width / 2, need.getGlobalBounds().height / 2);
					need.setFillColor(Color(96 * (choosenrebro != i) + 32 + abs(min((int(alltime * 0.1 + u + i * 100) % 762), 254) - 127.0),
						96 * (choosenrebro != i) + 32 + abs(min(max((int(alltime * 0.1 + u + i * 100) % 762) - 254.0, 0.0), 254.0) - 127.0),
						96 * (choosenrebro != i) + 32 + abs(min(max((int(alltime * 0.1 + u + i * 100) % 762) - 508.0, 0.0), 254.0) - 127.0), 24));
					need.setPosition(vershini[rebra[i].first].getPosition().x + (vershini[rebra[i].first].getGlobalBounds().width * 0.5) - u * xx,
						vershini[rebra[i].first].getPosition().y + (vershini[rebra[i].first].getGlobalBounds().height * 0.5) - u * yy);
					window.draw(need);
				}
				line.setFillColor(Color(128 * (choosenrebro != i) + abs(min((int(alltime * 0.1 + u + i * 100) % 762), 254) - 127.0),
					128 * (choosenrebro != i) + abs(min(max((int(alltime * 0.1 + u + i * 100) % 762) - 254.0, 0.0), 254.0) - 127.0),
					128 * (choosenrebro != i) + abs(min(max((int(alltime * 0.1 + u + i * 100) % 762) - 508.0, 0.0), 254.0) - 127.0)));
				line.setPosition(vershini[rebra[i].first].getPosition().x + (vershini[rebra[i].first].getGlobalBounds().width * 0.5) - u * xx,
					vershini[rebra[i].first].getPosition().y + (vershini[rebra[i].first].getGlobalBounds().height * 0.5) - u * yy);
				float finalscale = 1.0;
				for (int j = 0; j < muravi.size(); j++) if (i == muravi[j].placerebro)
					if (muravi[j].place == rebra[i].first)
						finalscale += -0.07 * min(int(abs(((muravi[j].nudolgoesho / rebra[i].vall) * float(dist)) - u) - rebra[i].vall * 16), 0);
					else
							finalscale += -0.07 * min(int(abs(dist - ((muravi[j].nudolgoesho / rebra[i].vall) * float(dist)) - u) - rebra[i].vall * 16), 0);
				if (finalscale >= 1.0 + rebra[i].vall * 16 * 0.07) finalscale = 1.0 + rebra[i].vall * 16 * 0.07;
				line.setScale(finalscale, finalscale);
				if (window.hasFocus()) if (Mouse::isButtonPressed(sf::Mouse::Left) && line.getGlobalBounds().contains(pos)) choosenrebro = i;
				window.draw(line);//
				line.setScale(1.0, 1.0);
			}
		}
		for (int i = 0; i < vershini.size(); i++)
		{
			for (int u = 0; u < FindedPath.vershini.size(); u++)
				if (FindedPath.vershini[u] == i)
				{
					CircleShape need(40);
					need.setOrigin(40, 40);
					need.setFillColor(Color(128 + abs(min((int(alltime * 0.2) % 762), 254) - 127.0),
						128 + abs(min(max((int(alltime * 0.2) % 762) - 254.0, 0.0), 254.0) - 127.0),
						128 + abs(min(max((int(alltime * 0.2) % 762) - 508.0, 0.0), 254.0) - 127.0), 64));
					need.setPosition(vershini[i].getPosition().x + vershini[i].getGlobalBounds().width / 2 - 5,
						vershini[i].getPosition().y + vershini[i].getGlobalBounds().height / 2 - 5);
					window.draw(need);
				}
			vershini[i].setOutlineColor(Color(128 + abs(min((int(alltime * 0.2) % 762), 254) - 127.0),
				128 + abs(min(max((int(alltime * 0.2) % 762) - 254.0, 0.0), 254.0) - 127.0),
				128 + abs(min(max((int(alltime * 0.2) % 762) - 508.0, 0.0), 254.0) - 127.0), 64));
			if (i == choosens[0] || i == choosens[1])
			{
				vershini[i].setOutlineColor(Color(128 + abs(min((int(alltime * 0.1) % 762), 254) - 127.0),
					128 + abs(min(max((int(alltime * 0.1) % 762) - 254.0, 0.0), 254.0) - 127.0),
					128 + abs(min(max((int(alltime * 0.1) % 762) - 508.0, 0.0), 254.0) - 127.0)));
			}
			window.draw(vershini[i]);
		}
		if (window.hasFocus()) if (choosenrebro != -1 && !Keyboard::isKeyPressed(Keyboard::LShift))
		{
			stringstream ss;
			ss << "Vall: " << int(rebra[choosenrebro].vall * 100) * 0.01;
			text.setString(ss.str());
			text.setPosition(10, 10);
			window.draw(text);
		}
		if (pathhh)
		{
			stringstream ss;
			ss << "Path Long: " << FindedPath.pathvall;
			text.setString(ss.str());
			text.setPosition(10, 70);
			window.draw(text);
		}
		text.setString(L"Мурашки");
		text.setPosition(10, ScreenSize[1] - 70);
		text.setFillColor(Color(0, 60 + 60 * murashki, 60 + 60 * murashki));
		text.setOutlineColor(Color(60 + 60 * murashki, 0, 60 + 60 * murashki));
		window.draw(text); 
		window.display();
		for (int j = 0; j < muravi.size(); j++) muravi[j].Move(TimesGone);
		if (window.hasFocus())
		{
			if (Mouse::isButtonPressed(sf::Mouse::Left) && mousecd >= 300)
				if (text.getGlobalBounds().contains(pos))
				{
					murashki = !murashki; mousecd = 0;
				}
			if (Keyboard::isKeyPressed(Keyboard::Delete) && choosenrebro >= 0)
			{
				rebra.erase(rebra.begin() + choosenrebro); choosenrebro = -1;
			}
			if (Mouse::isButtonPressed(sf::Mouse::Left) && mousecd >= 300)
			{
				mousecd = 0;
				bool find = false;
				for (int i = 0; i < vershini.size(); i++)
					if (vershini[i].getGlobalBounds().contains(pos))
					{
						if (Keyboard::isKeyPressed(Keyboard::LShift))
						{
							mousecd = 300; choosenrebro = -1;
							vershini[i].setPosition(pos.x - vershini[i].getGlobalBounds().width / 2,
								pos.y - vershini[i].getGlobalBounds().height / 2);
						}
						else
						{
							choosens[1 - choosenb] = i;
							choosenb = !choosenb; find = true;
						}
					}
				if (!find)
				{
					choosenrebro = -1;
					choosens[0] = -1;
					choosens[1] = -1;
				}
			}
			if (Keyboard::isKeyPressed(Keyboard::Escape)) window.close();
			if (Keyboard::isKeyPressed(Keyboard::Space) && choosens[0] != -1 && choosens[1] != -1 && choosens[0] != choosens[1])
			{
				//cout << "1" << endl;
				bool can = true;
				for (int i = 0; i < rebra.size(); i++)
					if ((rebra[i].first == choosens[0] && rebra[i].second == choosens[1]) ||
						(rebra[i].first == choosens[1] && rebra[i].second == choosens[0])) can = false;
				if (can) rebra.push_back(Rebro(choosens[0], choosens[1]));
				//cout << "2" << endl;
			}
			if (Keyboard::isKeyPressed(Keyboard::Enter) && vershini.size() > 1 && choosens[0] != -1 && choosens[1] != -1)
			{
				if (murashki)
				{
					muravi.clear(); for (int i = 0; i < rebra.size(); i++) rebra[i].himsled = 0;
					QofMurashki = int(vershini.size() * 1.5);
					for (int i = 0; i < QofMurashki; i++) muravi.push_back(Murashka(choosens[1], choosens[0]));
				}
				else
				{
					timer = 0; pathhh = true;
					FindedPath.vershini.clear();
					FindedPath = FindPath(choosens[0], choosens[1]);
				}
			}
			if (Keyboard::isKeyPressed(Keyboard::BackSpace))
			{
				if (murashki)
				{
					murashki = false; muravi.clear();
				}
				else
				{
					FindedPath.vershini.clear(); pathhh = false;
				}
			}
		}
	}
	return 0;
}