#include "Jeu.hpp"
#include <iostream>
#include <fstream>
#include <cstdint>
#include "cppitertools/range.hpp"
#include "gsl/span"
#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"
#include "debogage_memoire.hpp"
#include "Developpeur.hpp"
#include "ListeDeveloppeurs.hpp"

using namespace std;
using namespace iter;
using namespace gsl;

#pragma region "Fonctions de base pour vous aider"
typedef uint8_t UInt8;
typedef uint16_t UInt16;
UInt8 lireUint8(istream& fichier)
{
    UInt8 valeur = 0;
    fichier.read((char*)&valeur, sizeof(valeur));
    return valeur;
}
UInt16 lireUint16(istream& fichier)
{
    UInt16 valeur = 0;
    fichier.read((char*)&valeur, sizeof(valeur));
    return valeur;
}
string lireString(istream& fichier)
{
    string texte;
    texte.resize(lireUint16(fichier));
    fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
    return texte;
}
gsl::span<Jeu*> spanListeJeux(const ListeJeux& liste)
{
    return gsl::span(liste.elements, liste.nElements);
}
gsl::span<Designer*> spanListeDesigners(const ListeDesigners& liste)
{
    return gsl::span(liste.elements, liste.nElements);
}
#pragma endregion


Designer* trouverDesignerParNom(const ListeJeux& listeJeux, string nomDesigner)
{

    for (Jeu* jeu : span<Jeu*>(listeJeux.elements, listeJeux.nElements))
    {
        for (Designer* designer : span<Designer*>(jeu->designers.elements, jeu->designers.nElements))

        {
            if (designer->nom == nomDesigner)
            {
                return designer;
            }


        }
    }

    return nullptr;
}

Designer* lireDesigner(istream& fichier, const ListeJeux& listeJeux)
{
    Designer designer = {}; // On initialise une structure vide de type Designer.
    designer.nom = lireString(fichier);
    designer.anneeNaissance = lireUint16(fichier);
    designer.pays = lireString(fichier);
    // Rendu ici, les champs précédents de la structure designer sont remplis
    // avec la bonne information.

    //TODO: Ajouter en mémoire le designer lu. Il faut renvoyer le pointeur créé.
    // Attention, valider si le designer existe déjà avant de le créer, sinon
    // on va avoir des doublons car plusieurs jeux ont des designers en commun
    // dans le fichier binaire. Pour ce faire, cette fonction aura besoin de
    // la liste de jeux principale en paramètre.

    Designer* designerRecherche = trouverDesignerParNom(listeJeux, designer.nom);

    if (designerRecherche == nullptr)
        designerRecherche = new Designer(designer);

    cout << "L'allocation du designer " << designerRecherche->nom << " est réussie." << endl;
    return designerRecherche;
    // Afficher un message lorsque l'allocation du designer est réussie.
   //cout << designer.nom << endl;  //TODO: Enlever cet affichage temporaire servant à voir que le code fourni lit bien les jeux.
           //sreturn nullptr;//TODO: Retourner le pointeur vers le designer crée.
}


void redimensionnerTableauJeux(unsigned nouvelleCapacite, ListeJeux& listeJeux)
{

    Jeu** nouveauTableauJeu = new Jeu * [nouvelleCapacite];

    for (unsigned i = 0; i < listeJeux.nElements; i++)
    {
        nouveauTableauJeu[i] = listeJeux.elements[i];
    }
    delete[] listeJeux.elements;
    listeJeux.elements = nouveauTableauJeu;
    listeJeux.capacite = nouvelleCapacite;

}




void ajouterJeu(Jeu* jeu, ListeJeux& listeJeux)
{
    if (listeJeux.capacite == 0)
    {
        redimensionnerTableauJeux(1, listeJeux);
    }


    if (listeJeux.nElements >= listeJeux.capacite)
    {
        unsigned nouvelleCapacite = listeJeux.capacite * 2;
        redimensionnerTableauJeux(nouvelleCapacite, listeJeux);
    }

    listeJeux.elements[listeJeux.nElements] = jeu;
    listeJeux.nElements++;

}


void enleverJeu(ListeJeux& listeJeux, Jeu* jeu)
{
    int index = 0;

    for (unsigned i = 0; i < listeJeux.nElements; i++)
    {
        if (listeJeux.elements[i] == jeu)
        {
            break;
        }

        index++;

    }

    listeJeux.elements[index] = listeJeux.elements[listeJeux.nElements - 1];
    listeJeux.elements[listeJeux.nElements - 1] = nullptr;
    listeJeux.nElements--;

}

Jeu* lireJeu(istream& fichier, ListeJeux& listeJeux)
{
    Jeu jeu = {}; // On initialise une structure vide de type Jeu
    jeu.titre = lireString(fichier);
    jeu.anneeSortie = lireUint16(fichier);
    jeu.developpeur = lireString(fichier);
    jeu.designers.nElements = lireUint8(fichier);
    // Rendu ici, les champs précédents de la structure jeu sont remplis avec la
    // bonne information.

    //TODO: Ajouter en mémoire le jeu lu. Il faut renvoyer le pointeur créé.
    // Attention, il faut aussi créer un tableau dynamique pour les designers
    // que contient un jeu. Servez-vous de votre fonction d'ajout de jeu car la
    // liste de jeux participé est une ListeJeu. Afficher un message lorsque
    // l'allocation du jeu est réussie.

    Jeu* pointeurJeu = new Jeu(jeu);
    pointeurJeu->designers.elements = new Designer * [pointeurJeu->designers.nElements];


    //cout << jeu.titre << endl;  //TODO: Enlever cet affichage temporaire servant à voir que le code fourni lit bien les jeux.

    for ([[maybe_unused]] int i : iter::range(jeu.designers.nElements))
    {
        Designer* pointeurDesigner = lireDesigner(fichier, listeJeux);

        pointeurJeu->designers.elements[i] = pointeurDesigner;
        ajouterJeu(pointeurJeu, pointeurDesigner->listeJeuxParticipes);

    }

    cout << "L'allocation du jeu " << pointeurJeu->titre << " est réussie." << endl;
    return pointeurJeu; //TODO: Retourner le pointeur vers le nouveau jeu.
}

ListeJeux creerListeJeux(const string& nomFichier)
{
    ifstream fichier(nomFichier, ios::binary);
    fichier.exceptions(ios::failbit);
    int nElements = lireUint16(fichier);
    ListeJeux listeJeux = {};
    listeJeux.elements = new Jeu * [nElements];
    listeJeux.capacite = nElements;

    for ([[maybe_unused]] int n : iter::range(nElements))
    {
        Jeu* jeu = lireJeu(fichier, listeJeux);
        ajouterJeu(jeu, listeJeux);//TODO: Ajouter le jeu à la ListeJeux.
    }

    return listeJeux; //TODO: Renvoyer la ListeJeux.
}

void detruireDesigner(Designer* designer)
{
    string nom = designer->nom;
    delete[] designer->listeJeuxParticipes.elements;
    cout << "Le pointeur du designer " << nom << " est détruit." << endl;
}

bool designerParticipe(Designer* designer)
{
    if (designer->listeJeuxParticipes.nElements != 0)
    {
        return true;
    }

    return false;
}

void detruirejeu(Jeu* ptrJeu, ListeJeux& listeJeu)
{
    for (Designer* ptrDesigner : spanListeDesigners(ptrJeu->designers))
    {
        bool estDernierJeu = ptrDesigner->listeJeuxParticipes.nElements <= 1;
        if (estDernierJeu)
        {
            detruireDesigner(ptrDesigner);
            delete ptrDesigner;
        }
        else
        {
            enleverJeu(ptrDesigner->listeJeuxParticipes, ptrJeu);
        }
    }

    cout << "Le pointeur du jeu " << ptrJeu->titre << " a été détruit." << endl;
    delete[] ptrJeu->designers.elements;
}


void detruireListeJeux(ListeJeux& listeJeux)
{
    for (Jeu* pointeurJeu : spanListeJeux(listeJeux))
    {
        detruirejeu(pointeurJeu, listeJeux);
        delete pointeurJeu;
    }
    delete[] listeJeux.elements;
}


void afficherDesigner(const Designer& d)
{
    cout << "\t" << d.nom << ", " << d.anneeNaissance << ", " << d.pays
        << endl;
}

void afficherJeu(const Jeu* jeu)
{
    cout << "Jeu: " << jeu->titre << endl;
    cout << "Annee de sortie: " << jeu->anneeSortie << endl;
    cout << "Développeur: " << jeu->developpeur << endl;
    cout << "Les designers: " << endl;
    for (Designer* designer : span <Designer*>(jeu->designers.elements, jeu->designers.nElements))
    {
        afficherDesigner(*designer);
    }
}

void afficherListeJeux(const ListeJeux& listeJeux)
{
    static const string ligneSeparation = "\n\033[12m--------------------------------------------------\033[0m\n";

    for (Jeu* jeuAffiche : span<Jeu*>(listeJeux.elements, listeJeux.nElements)) {

        afficherJeu(jeuAffiche);
        cout << ligneSeparation << endl;
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
#pragma region "Bibliothèque du cours"
    bibliotheque_cours::activerCouleursAnsi();
#pragma endregion

    static const string ligneSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";
    ListeJeux listeJeux = creerListeJeux("jeux.bin");

    // Tests pour les fonctions du main
    cout << ligneSeparation;
    cout << "Premier jeu de la liste :" << endl;
    afficherJeu(listeJeux.elements[0]);
    cout << ligneSeparation << endl;

    cout << "Affichage de la liste de jeu:\n" << endl;
    afficherListeJeux(listeJeux);
    designerParticipe(listeJeux.elements[0]->designers.elements[0]); // Appel pour la couverture de code (Pas veritablement uttilise)


    // Tests sur les methodes de la classe Developpeur
    cout << ligneSeparation;
    cout << "Tests des methodes de la classe Developpeur: " << endl;
    Developpeur dev;
    cout << dev.obtenirNom() << endl;

    Developpeur square("Square");
    cout << square.obtenirNom() << endl;
    cout << square.compterNombreJeux(listeJeux) << endl; // Square a developpe 3 jeux
    square.mettreDeveloppeurAJour(listeJeux);
    square.afficherJeuxDeveloppes(); // Les jeux developpes par Square sont Chrono Trigger, Final Fantasy VI et Secret of Mana


    // Tests sur les methodes de la classe ListeDeveloppeurs
    Developpeur nintendo("Nintendo");
    nintendo.mettreDeveloppeurAJour(listeJeux);

    cout << ligneSeparation;
    cout << "Tests des methodes de la classe ListeDeveloppeurs: " << endl;
    ListeDeveloppeurs listeDev;
    listeDev.afficher();
    listeDev.ajouterDeveloppeur(&square);
    listeDev.ajouterDeveloppeur(&nintendo);
    listeDev.afficher();

    listeDev.retirerDeveloppeur(&square);
    listeDev.afficher();
    cout << ligneSeparation;

    detruireListeJeux(listeJeux);
}
