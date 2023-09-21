#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

struct klocek {
  int rozmiar, wysokosc, szerokosc; // Liczba pol nalezacych do klocka
  char litera;
  vector<string> ksztalt;
  pair<int, int>
      rog; // wspolrzedne lewego gornego rogu najmniejszego prostokąta
           // w ktory mozna wpisac daną figurę (first - wysokosc)
  bool czy90obracalny, czy90id, czy180id;
};

vector<klocek> klocki[4];
vector<string> pusta_plansza;
vector<int> pozostalo_rozmiarow; // wektor ktory przechowuje sume rozmiarow
                                 // pozostalych klockow
bool czyZnaleziono;
int n, m, k;

bool porownajKlocki(klocek a, klocek b) {
  if (a.rozmiar != b.rozmiar)
    return a.rozmiar > b.rozmiar;
  return a.litera < b.litera;
}

bool czySieZmiesci(vector<string> plansza, klocek klocek,
                   pair<int, int> wspolrzedne) {
  for (int i = 0; i < klocek.wysokosc; i++) {
    for (int j = 0; j < klocek.szerokosc; j++) {
      if (wspolrzedne.first + i >= n || wspolrzedne.second + j >= m){
        return false;
      }
      if (klocek.ksztalt[i][klocek.rog.second + j] != '.' && plansza[wspolrzedne.first + i][wspolrzedne.second + j] != '.'){
        return false;
      }
    }
  }
  return true;
}

vector<string> umiesc_klocek_na_planszy(vector<string> plansza, klocek a,
                                        pair<int, int> wspolrzedne) {
  vector<string> res = plansza;
  for (int i = 0; i < a.wysokosc; i++) {
    for (int j = 0; j < a.szerokosc; j++) {
      if (a.ksztalt[i][a.rog.second + j] != '.')
        res[wspolrzedne.first + i][wspolrzedne.second + j] =
            a.ksztalt[i][a.rog.second + j];
    }
  }
  return res;
}

// Sprawdza, czy klocek daje sie obrocic o 90 stopni tak, zeby dalej sie miescil
bool czyObracalny(klocek dany) {
  if (dany.wysokosc > m || dany.szerokosc > n)
    return false;
  return true;
}

klocek obroc90(klocek dany) {
  klocek res;
  res.rog = make_pair(0, 0);
  res.wysokosc = dany.szerokosc;
  res.szerokosc = dany.wysokosc;
  res.rozmiar = dany.rozmiar;
  res.ksztalt = pusta_plansza;
  res.czy90obracalny = dany.czy90obracalny;
  res.czy90id = dany.czy90id;
  res.czy180id = dany.czy180id;
  res.litera = dany.litera;
  for (int i = 0; i < dany.szerokosc; i++) {
    for (int j = 0; j < dany.wysokosc; j++) {
      res.ksztalt[i][j] =
          dany.ksztalt[j][dany.rog.second + dany.szerokosc - 1 - i];
    }
  }
  return res;
}

klocek obroc270(klocek dany) {
  klocek res;
  res.rog = make_pair(0, 0);
  res.wysokosc = dany.szerokosc;
  res.szerokosc = dany.wysokosc;
  res.rozmiar = dany.rozmiar;
  res.ksztalt = pusta_plansza;
  res.czy90obracalny = dany.czy90obracalny;
  res.czy90id = dany.czy90id;
  res.czy180id = dany.czy180id;
  res.litera = dany.litera;
  for (int i = 0; i < dany.szerokosc; i++) {
    for (int j = 0; j < dany.wysokosc; j++) {
      res.ksztalt[i][j] = dany.ksztalt[dany.wysokosc - 1 - j][dany.rog.second + i];
    }
  }
  return res;
}

klocek obroc180(klocek dany) {
  klocek res;
  res.rog = make_pair(0, 0);
  res.wysokosc = dany.wysokosc;
  res.szerokosc = dany.szerokosc;
  res.rozmiar = dany.rozmiar;
  res.ksztalt = pusta_plansza;
  res.czy90obracalny = dany.czy90obracalny;
  res.czy90id = dany.czy90id;
  res.czy180id = dany.czy180id;
  res.litera = dany.litera;
  for (int i = 0; i < dany.wysokosc; i++) {
    for (int j = 0; j < dany.szerokosc; j++) {
      res.ksztalt[i][j] =
          dany.ksztalt[dany.wysokosc - i - 1]
                      [dany.rog.second + dany.szerokosc - 1 - j];
    }
  }
  return res;
}

// Sprawdza czy a, oraz a obrocony o 180 stopni sa takie same
bool czy180identyczny(klocek a) {
  for (int i = 0; i < (a.wysokosc + 1) / 2; i++) {
    for (int j = 0; j < (a.szerokosc + 1) / 2; j++) {
      if (a.ksztalt[i][j] !=
          a.ksztalt[a.wysokosc - i - 1][a.rog.second + a.szerokosc - 1 - j])
        return false;
    }
  }
  return true;
}

// Sprawdza czy a, oraz a obrocony o 90 stopni sa takie same
bool czy90identyczny(klocek a) {
  if (a.wysokosc != a.szerokosc)
    return false;
  for (int i = 0; i < (a.szerokosc + 1) / 2; i++) {
    for (int j = 0; j < (a.wysokosc + 1) / 2; j++) {
      if (a.ksztalt[i][j] != a.ksztalt[j][a.rog.second + a.szerokosc - 1 - i])
        return false;
    }
  }
  return true;
}

void solve(int ind, vector<string> plansza, int zapelnionych) {
  if (czyZnaleziono)
    return;
  if (zapelnionych == n * m) {
    czyZnaleziono = true;
    cout << "TAK" << endl;
    for (int i = 0; i < int(plansza.size()); i++)
      cout << plansza[i] << endl;
    return;
  }
  if (ind >= k)
    return;
  if (klocki[0][ind].rozmiar + zapelnionych > n * m)
    return;
  klocek a = klocki[0][ind];
  for (int i = 0; i + a.wysokosc - 1 < n; i++) {
    for (int j = 0; j + a.szerokosc - 1 < m; j++) {
      for (int o = ind + 1; o <= k; o++) {
        if (pozostalo_rozmiarow[o] + zapelnionych + a.rozmiar < n * m)
          break;
        if (czySieZmiesci(plansza, a, make_pair(i, j))){
          solve(o, umiesc_klocek_na_planszy(plansza, a, make_pair(i, j)),
                zapelnionych + a.rozmiar);
        }
      }
    }
  }
  if (a.czy90id)
    return;
  if (a.czy180id) {
    for (int rota = 1; rota <= 3; rota += 2) {
      a = klocki[rota][ind];
      for (int i = 0; i + a.wysokosc - 1 < n; i++) {
        for (int j = 0; j + a.szerokosc - 1 < m; j++) {
          for (int o = ind + 1; o <= k; o++) {
            if (pozostalo_rozmiarow[o] + zapelnionych + a.rozmiar < n * m)
              break;
            if (czySieZmiesci(plansza, a, make_pair(i, j))){
              solve(o, umiesc_klocek_na_planszy(plansza, a, make_pair(i, j)),
                    zapelnionych + a.rozmiar);
            }
          }
        }
      }
    }
    return;
  }
  if (a.czy90obracalny) {
    for (int rota = 1; rota <= 3; rota++) {
      a = klocki[rota][ind];
      for (int i = 0; i + a.wysokosc - 1 < n; i++) {
        for (int j = 0; j + a.szerokosc - 1 < m; j++) {
          for (int o = ind + 1; o <= k; o++) {
            if (pozostalo_rozmiarow[o] + zapelnionych + a.rozmiar < n * m)
              break;
            if (czySieZmiesci(plansza, a, make_pair(i, j))){
              solve(o, umiesc_klocek_na_planszy(plansza, a, make_pair(i, j)),
                    zapelnionych + a.rozmiar);
            }
          }
        }
      }
    }
  } else {
    a = klocki[2][ind];
    for (int i = 0; i + a.wysokosc - 1 < n; i++) {
      for (int j = 0; j + a.szerokosc - 1 < m; j++) {
        for (int o = ind + 1; o <= k; o++) {
          if (pozostalo_rozmiarow[o] + zapelnionych + a.rozmiar < n * m)
            break;
          if (czySieZmiesci(plansza, a, make_pair(i, j)))
            solve(o, umiesc_klocek_na_planszy(plansza, a, make_pair(i, j)),
                  zapelnionych + a.rozmiar);
        }
      }
    }
  }
  return;
}

int main() {
  cin >> n >> m >> k;
  string tmp;
  getline(cin, tmp);
  string wiersz;
  char znak;
  klocek kloc;
  int obecna_szerokosc;
  int sumaPol = 0;
  for (int i = 0; i < n; i++) {
    wiersz = "";
    for (int j = 0; j < m; j++) {
      wiersz += '.';
    }
    pusta_plansza.push_back(wiersz);
  }
  for (int i = 0; i < k; i++) {
    kloc.rog.first = n;
    kloc.rog.second = m;
    kloc.rozmiar = 0;
    kloc.szerokosc = 0;
    kloc.wysokosc = 0;
    kloc.ksztalt.clear();
    kloc.czy90obracalny = false;
    kloc.czy90id = false;
    kloc.czy180id = false;
    kloc.litera = '.';
    for (int j = 0; j < n; j++) {
      wiersz = "";
      obecna_szerokosc = 0;
      for (int l = 0; l < m; l++) {
        cin >> znak;
        wiersz += znak;
        if (znak != '.') {
          obecna_szerokosc++;
          kloc.litera = znak;
          kloc.rozmiar++;
          kloc.rog.second = min(kloc.rog.second, l);
        }
      }
      if (obecna_szerokosc != 0) {
        if (kloc.wysokosc == 0)
          kloc.rog.first = j;
        kloc.wysokosc++;
        sumaPol += kloc.rozmiar;
        kloc.ksztalt.push_back(wiersz);
      }
    }

    for (int l = 0; l < kloc.wysokosc; l++) {
      for (int o = 0; o < m; o++) {
        if (kloc.ksztalt[l][o] != '.') {
          kloc.szerokosc = max(kloc.szerokosc, o - kloc.rog.second + 1);
        }
      }
    }
    // czy90obracalny, czy90id, czy180id;
    kloc.czy90obracalny = czyObracalny(kloc);
    kloc.czy90id = czy90identyczny(kloc);
    kloc.czy180id = czy180identyczny(kloc);
    klocek a;
    klocki[0].push_back(kloc);
    if (kloc.czy90obracalny) {
      a = obroc90(kloc);
      klocki[1].push_back(a);
      a = obroc270(kloc);
      klocki[3].push_back(a);
    } else {
      klocki[1].push_back(kloc);
      klocki[3].push_back(kloc);
    }
    a = obroc180(kloc);
    klocki[2].push_back(a);
    if (i != k - 1)
      getline(cin, tmp);
  }


  if (sumaPol < n * m) {
    cout << "NIE\n";
    return 0;
  }
  sort(klocki[0].begin(), klocki[0].end(), porownajKlocki);
  sort(klocki[1].begin(), klocki[1].end(), porownajKlocki);
  sort(klocki[2].begin(), klocki[2].end(), porownajKlocki);
  sort(klocki[3].begin(), klocki[3].end(), porownajKlocki);
  pozostalo_rozmiarow.resize(k + 1);
  pozostalo_rozmiarow[k] = 0;
  for (int i = k - 1; i >= 0; i--) {
    pozostalo_rozmiarow[i] = klocki[0][i].rozmiar + pozostalo_rozmiarow[i + 1];
  }

  czyZnaleziono = false;
  for (int i = 0; i < k; i++) {
    solve(i, pusta_plansza, 0);
  }
  if (!czyZnaleziono)
    cout << "NIE\n";
  return 0;
}
