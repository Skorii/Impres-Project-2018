#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <time.h>


struct Date
{
    short Jour;
    short Mois;
    short Annee;
};

struct Acheter
{
    char IdProduit[14];
    long IdFournisseur;
    struct Date DateAchat;
    struct Date DatePeremption;
    float PrixUnitaire;
    int Quantite;
};

struct IndDate
{
    struct Date DatePeremption;
    char IdProduit[14];
    int Quantite;
    long posi;
};

struct IndQuant
{
    int Quantite;
    char IdProduit[14];
};

struct Fournisseur
{
    long IdFournisseur;
    char NomFournisseur[50];
    char VilleFournisseur[40];
};

struct Produit
{
    char IdProduit [14];
    char Marque[50];
    char NomProduit[50];
    char Volume[9];
    char categorie[40];
    long posi;
};


char Menu ();
char MenuFourni(int);
char MenuAchat(int);
int EncodeAchat(struct Acheter *,long [],int); // Encode les achat dans un vecteur de structure
int VerifDate (short,short,short); // Verifie si la date existe bien
int ValiditeAchat (struct Acheter *, struct Acheter *, int);// Verifie si l'acaht n'a pas déja ete effectuer
void AfficheAchat (int);// Permet d'afficher le vecteur d'achats dans l'ordre d'encodage
int ComparaisonDate(struct Date, struct Date); // Compare les dates pour voir si l'achat peut etre valider
int Validite(struct Acheter temp, int); //Verifie dans le vecteur si l'achat n'a pas été éffectué
int SoustractionDate(struct Date Date1, struct Date Date2);// Compte le nombre de jours entre les 2 dates donnée en parametre
void AfficheIndex(struct IndDate index[], int nachat); // Affiche les articles trié en fonction de la date de peremption
void InsertionIndDate (struct Acheter temp, struct IndDate IndexDate[], int nachat);// Insert dans IndexDate les elements par ordre de peremption
int RechercheIndQuant(struct IndQuant indexQuant[],struct Acheter temp,int *);// cherche si l'id est presente et ret -1 OU la position de l'id double
int SuppressionIndQuant(struct IndQuant indexQuant[],int *,struct Acheter temp, int);// décale tout vers la gauche pour supprimer, retourne la quantité a ajouter
void InsertionIndQuant(struct IndQuant indexQuant[],int retQuant,struct Acheter temp,int *);//insert dans indexQuant
void AfficheQuant(struct IndQuant indexQuant[], int );
void InsertionIndexFournisseur(struct Fournisseur, long [],int*);
int VerifFournisseur(long [], long, int);
void CreationFichierProduit();
char MenuProduit(int);
struct Produit EncodeProduit(int);
void AfficheUnProduit(struct Produit);
void EcritureFichier(struct Produit,int);
int StrCmpCaMaNo (struct Produit, struct Produit);
int isNbr (char []);
int isCharStr (char []);
struct Produit StrToLower(struct Produit);
int VerifID (char [], int);
int VerifVolume(char[]);
long getptl();
long getpto();
void writeptl(long);
void writepto(long);

void main()
{
    struct Acheter temp;
    struct Produit tempProd;
    struct IndDate IndexDate[5000];
    struct IndQuant indexQuant[5000];
    struct Fournisseur fourni;
    char choix;
    int nachat=0,menu,quitterProg=0,quitter=0,next, nbrIndexQuant=0,retQuant,retPos,nProduit=0,quitterProduit=0,i=0,quitterFournisseur=0;
    long IdFourni[100]={0}, rechID,posi; // Vecteur contenant les ID de tous les fournisseurs le vecteur est trillé par ordre croissant
    int nFourni=0,trouve,IdDejaPres=0;
    int *pnFourni = &nFourni;
    FILE * fProduit = NULL;
    FILE * fFournisseur = NULL;
    FILE * fAchat = NULL;

    /********** Remplisage des vecteurs **********/
    fFournisseur = fopen("Fournisseurs.dat","rb");
    if (fFournisseur != NULL)
    {
        while(fread(&fourni,sizeof(struct Fournisseur),1,fFournisseur))
        {
            InsertionIndexFournisseur(fourni,IdFourni,pnFourni);
        }
        fclose(fFournisseur);
    }
    else
    {
        fFournisseur = fopen("Fournisseurs.dat","wb");
        printf("  >> INFO : Le fichier Fournisseurs.dat a ete cree avec succes !\n");
    }

    fAchat = fopen("Achats.dat","rb");
    if (fAchat != NULL)
    {
        while(fread(&temp,sizeof(struct Acheter),1,fAchat))
        {
            nachat++;
        }
    }
    else
    {
        fFournisseur = fopen("Achats.dat","wb");
        printf("  >> INFO : Le fichier Achats.dat a ete cree avec succes !\n");
    }

    fProduit = fopen("Produits.dat","rb");
    if(fProduit == NULL)
    {
        CreationFichierProduit();
    }
    else
    {
        nProduit = (int)getptl();
    }
    fclose(fProduit);
    /********** Les vecteurs sont remplis **********/
    do
    {
        menu = Menu();
        system("cls");
        switch(menu)
        {
            case '1' : /********** MENU ACHAT **********/
                do
                {
                    menu = MenuAchat(nachat);
                    next = 1;
                    switch(menu)
                    {
                        case '1':
                            system("cls");
                            if(nFourni > 0)
                            {
                                while (nachat<5000 && nFourni > 0 && EncodeAchat(&temp,IdFourni,nFourni))
                                {
                                    if (Validite(temp,nachat))
                                    {
                                        if(nachat == 0)
                                        {
                                            fAchat = fopen("Achats.dat","wb");
                                        }
                                        fAchat = fopen("Achats.dat","a+b");
                                        if(fAchat == NULL)
                                        {
                                            printf("  >> ERREUR : Impossible d'ouvrir le fichier Achats.dat.\n");
                                        }
                                        else
                                        {
                                            //fseek(fAchat,nachat*sizeof(struct Acheter),SEEK_SET);
                                            fwrite(&temp,sizeof(struct Acheter),1,fAchat);
                                            fclose(fAchat);
                                        }
                                        // encoder fichier achat
                                        InsertionIndDate(temp,IndexDate,nachat);
                                        retPos = RechercheIndQuant(indexQuant,temp,&nbrIndexQuant);
                                        if (retPos >= 0)
                                        {
                                            retQuant = SuppressionIndQuant(indexQuant,&nbrIndexQuant,temp, retPos);
                                        }
                                        InsertionIndQuant(indexQuant,retQuant,temp,&nbrIndexQuant);
                                        printf("=== Article ajoute. ===\n\n");
                                        nachat++;
                                    }
                                    else
                                    {
                                        printf(" >> ERREUR : Impossible d'ajouter cet achat.\n\n");
                                    }
                                }
                            }
                            else
                            {
                                system("cls");
                                printf("  >> ERREUR : Impossible d'encoder un achat, si aucun fournisseur n'existe.\n");
                            }

                            break;
                        case '2':
                            system("cls");
                            i=0;
                            if (nachat == 0)
                            {
                                printf(" >> ERREUR : Aucun achat n'a encore ete effectue.\n");
                            }
                            else
                            {
                                printf("Voici la liste des achats. ""Entrer"" pour continuer, autre touche pour quitter.\nNombre d'achats encodes : %d\n\n",nachat);
                                do
                                {
                                    AfficheAchat(i);
                                    i++;
                                }
                                while(i<nachat && getch() == 13);
                            }
                            system("cls");
                            break;
                        case '3':
                            system("cls");
                            if (nachat == 0)
                            {
                                printf(" >> ERREUR : Aucun achat n'a encore ete effectue.\n");
                            }
                            else
                            {
                                printf("Voici la liste des achats par ordre croissant de date de peremption.\n ENTER pour afficher l'element suivant.\nNombre d'achats encodes : %d\n\n",nachat);
                                AfficheIndex(IndexDate, nachat);
                            }
                            system("cls");
                            break;
                        case '4':
                            system("cls");
                            i=0;
                            printf("Voici la liste des quantite par ordre croissant. \nENTER pour afficher l'element suivant.\nNombre de quantite encodes : %d\n\n",nbrIndexQuant);
                            if (nbrIndexQuant == 0)
                            {
                                printf(" >> ERREUR : Aucun achat n'a encore ete effectue.\n");
                            }
                            else
                            {
                                do
                                {
                                    AfficheQuant(indexQuant,i);
                                    i++;
                                }
                                while(i<nbrIndexQuant);
                            }
                            system("cls");

                            break;
                        case '5':

                            break;
                        case '6':
                            quitter = 1;
                            system("cls");
                            break;
                        default : ;
                    }
                }
                while(!quitter);
                break;
            case '2' : /********** MENU FOURNISSEURS **********/
                do
                {
                    menu = MenuFourni(nFourni);
                    switch(menu)
                    {
                        case '1' : // Ajouter un fournisseur en fin de fichier.
                            fFournisseur = fopen("Fournisseurs.dat","a+b");
                            system("cls");
                            do
                            {
                                next = 0;
                                printf("Entrez l'ID du nouveau fournisseur : ");
                                scanf("%ld",&fourni.IdFournisseur);
                                i=0;
                                while (i<nFourni && fourni.IdFournisseur <= IdFourni[i] && !IdDejaPres)
                                {
                                    if(fourni.IdFournisseur == IdFourni[i])
                                    {
                                        IdDejaPres = 1;
                                    }
                                    i++;
                                }
                                if (!IdDejaPres)
                                {
                                    if(fourni.IdFournisseur != 0)
                                    {
                                        printf("Entrez le nom du nouveau fournisseur : ");
                                        fflush(stdin);
                                        gets(fourni.NomFournisseur);
                                        printf("Entrez la ville du nouveau fournisseur : ");
                                        fflush(stdin);
                                        gets(fourni.VilleFournisseur);
                                        if (fwrite (&fourni, sizeof(struct Fournisseur), 1, fFournisseur))
                                        {
                                            InsertionIndexFournisseur(fourni, IdFourni,pnFourni);
                                            printf("Ajoute effectue!");
                                        }
                                        printf("\nEnter pour continuer l'encodage.\n\n");
                                    }
                                    if(getch()==13)
                                    {
                                        next = 1;
                                    }
                                    else
                                    {
                                        system("cls");
                                    }
                                }
                                else
                                {
                                    system("cls");
                                    printf("  >> ERREUR : L'ID saisi est deja encode.\n\n");
                                }
                            }
                            while (next == 1);
                            if (fFournisseur!= NULL)
                            {
                                fclose(fFournisseur);
                            }
                            break;
                        case '2' : // Modification de la fiche fournisseur. Rechercher puis permettre modification.
                            system("cls");
                            fFournisseur = fopen("Fournisseurs.dat","a+b");
                            printf("Entrez l'ID du Fournisseur a rechercher : ");
                            scanf("%ld",&rechID);
                            i=0;
                            trouve = 0;
                            while (i<nFourni && rechID >= IdFourni[i] && trouve == 0)
                            {
                                if(rechID == IdFourni[i])
                                {
                                    trouve = 1;
                                }
                                i++;
                            }
                            if(trouve)
                            {
                                i=0;
                                system("cls");
                                do
                                {
                                    fread(&fourni,sizeof(struct Fournisseur),1,fFournisseur);
                                    i++;
                                }
                                while (fourni.IdFournisseur != rechID);
                                if (fFournisseur!= NULL)
                                {
                                    fclose(fFournisseur);
                                }
                                fFournisseur = fopen("Fournisseurs.dat","r+b");
                                printf("Id du Fournisseur : %ld\nNom du Fournisseur : ", fourni.IdFournisseur);
                                puts(fourni.NomFournisseur);
                                printf("Ville du Fournisseur : ");
                                puts(fourni.VilleFournisseur);
                                printf("Voulez vous modifier ce Fournisseur ? (Enter pour valider).\n");
                                if(getch() == 13)
                                {
                                    printf("Entrez le nouveau nom de fournisseur : ");
                                    fflush(stdin);
                                    gets(fourni.NomFournisseur);
                                    printf("Entrez la nouvelle ville du Fournisseur : ");
                                    fflush(stdin);
                                    gets(fourni.VilleFournisseur);
                                    fseek(fFournisseur,(i-1)*sizeof(struct Fournisseur),SEEK_SET);
                                    fwrite (&fourni,sizeof(struct Fournisseur), 1, fFournisseur);
                                }
                                if (fFournisseur!= NULL)
                                {
                                    fclose(fFournisseur);
                                }
                                system("cls");
                            }
                            else
                            {
                                if (fFournisseur!= NULL)
                                {
                                    fclose(fFournisseur);
                                }
                                system("cls");
                                printf("  >> ERREUR : Le Fournisseur n'a pas ete trouve\n\n");
                            }
                            break;
                        case '3' : // Affichage sequentielle des fournisseurs.
                            if(nFourni > 0)
                            {
                                system("cls");
                                printf("Voici la liste de vos Fournisseurs. Appuyez sur Enter pour afficher le suivant.\n");
                                if((fFournisseur = fopen("Fournisseurs.dat","rb")) != NULL)
                                {
                                    i=0;
                                    do
                                    {
                                        next = 0;
                                        fread(&fourni,sizeof(struct Fournisseur),1,fFournisseur);
                                        printf("\nId du Fournisseur : %ld\nNom du Fournisseur : ", fourni.IdFournisseur);
                                        puts(fourni.NomFournisseur);
                                        printf("Ville du Fournisseur : ");
                                        puts(fourni.VilleFournisseur);
                                        if(getch()==13)
                                        {
                                            next = 1;
                                        }
                                        i++;
                                    }
                                    while(next == 1 && i < nFourni);
                                    system("cls");
                                }
                                if (fFournisseur!= NULL)
                                {
                                    fclose(fFournisseur);
                                }
                            }
                            else
                            {
                                system("cls");
                                printf("  >> ERREUR : Aucun Fournisseur n'est encore encode\n\n");
                            }
                            break;
                        case '4' :
                            quitterFournisseur = 1;
                            system("cls");
                            break;
                        default :
                            system("cls");
                            printf("  >> ERREUR : Choix non possible...\n");
                    }
                }
                while(!quitterFournisseur);
                system("cls");
                break;

            case '3' :/********** MENU PRODUIT **********/
                while(!quitterProduit)
                {
                    choix = MenuProduit(nProduit);
                    switch(choix)
                    {
                        case '1' :
                            system("cls");
                            do
                            {
                                tempProd = EncodeProduit(nProduit);
                                if(tempProd.IdProduit[0] != '\0')
                                {
                                    EcritureFichier(tempProd, nProduit);
                                    nProduit++;
                                }
                                printf("ENTER pour continuer l'encodage.\n\n");
                            }
                            while (getch() == 13);
                            system("cls");
                            break;
                        case '2' :
                            system("cls");
                            i=0;
                            fProduit = fopen("Produits.dat","rb");
                            if(fProduit == NULL)
                            {
                                printf("  >> ERREUR : Impossible d'ouvrir le fichier...\n");
                            }
                            else
                            {
                                if(nProduit == 0)
                                {
                                    printf("  >> ERREUR : Aucun produit encode...\n");
                                }
                                else
                                {
                                    posi = getpto();
                                    fseek(fProduit,0,SEEK_SET);
                                    printf("  >> INFO : ENTER pour affichier l'element suivant.\n");
                                    do
                                    {
                                        fseek(fProduit,2*sizeof(long)+posi*sizeof(struct Produit),SEEK_SET);
                                        fread(&tempProd,sizeof(struct Produit),1,fProduit);
                                        AfficheUnProduit(tempProd);
                                        posi = tempProd.posi;
                                        i++;
                                    }
                                    while (getch() == 13 && i<nProduit && posi != -1);
                                    system("cls");
                                }
                                fclose(fProduit);
                            }
                            while (i<nProduit && getch() == 13);
                            break;
                        case '3' :
                            system("cls");
                            quitterProduit = 1;
                            break;
                        default :
                            system("cls");
                            printf("  >> ERREUR : Choix non possible...\n");
                    }
                }
                break;
            case '4' : /********** QUITTER **********/
                quitterProg = 1;
                break;

        }
    }
    while(!quitterProg);
}

/*****************************************************************************************************************************************************************************/

char Menu()
{
    char choix[20];
    printf("  === MENU PRINCIPAL ===");
    printf("\n 1) Achats.");
    printf("\n 2) Fournisseurs.");
    printf("\n 3) Produits.");
    printf("\n 4) Quitter.\n\n");
    printf("  >> Votre choix : ");
    fflush(stdin);
    gets(choix);
    if(strlen(choix)>1)
    {
        return '9';
    }
    return choix[0];
}

/*****************************************************************************************************************************************************************************/

char MenuAchat (int nachat)
{
    char choix[20];
    printf("  === MENU ACHAT ===");
    printf("\n 1) Ajouter des achats.");
    printf("\n 2) Afficher les achats dans l'ordre d'encodage.");
    printf("\n 3) Afficher les achats dans l'ordre croissant de date de peremption.");
    printf("\n 4) Afficher les produit par quantite croissante.");
    printf("\n 5) Consommer un produit.");
    printf("\n 6) Retour au menu principale.\n\n");
    printf("  === Il y a %d achat(s) effectue(s). ===\n\n",nachat);
    printf("\n  >> Votre choix : ");
    fflush(stdin);
    gets(choix);
    if(strlen(choix)>1)
    {
        return '9';
    }
    return choix[0];
}

/*****************************************************************************************************************************************************************************/

char MenuFourni (int nFourni)
{
    char choix[20];
    printf("  === MENU FOURNISSEUR ===");
    printf("\n 1) Ajouter un fournisseur.");
    printf("\n 2) Modifier la fiche d'un fournisseur.");
    printf("\n 3) Affichage des fournisseurs.");
    printf("\n 4) Retour au menu principale.\n");
    printf("\n  === Il y a %d fournisseur(s) encode(s). ===\n\n",nFourni);
    printf("\n  >> Votre choix : ");
    fflush(stdin);
    gets(choix);
    if(strlen(choix)>1)
    {
        return '9';
    }
    return choix[0];
}

/*****************************************************************************************************************************************************************************/

int EncodeAchat(struct Acheter *ptemp,long IdFourni[],int nFourni)
{
    FILE * fFournisseur = NULL;
    int idcorrect=0,i=0,fourni=0,DateCorrect = 0,prixCorrect=0,carac='.',quant=0;
    short j,m,a,jp,mp,ap;
    char date[10],prix[10];
    char *p;
    // Id Produit
    while (idcorrect == 0)
    {
        printf("Entrez l'ID du produit : ");
        fflush(stdin);
        gets(ptemp->IdProduit);
        while (ptemp->IdProduit[i]!='\0')
        {
            i++;
        }
        if(ptemp->IdProduit[0] == '\0')
        {
            system("cls");
            //printf("=== Aucun article n'a ete ajoute. ===\n\n");
            return 0;
        }
        if (i == 13)
        {
            idcorrect = 1;
        }
        else
        {
            printf("L'ID saisi est incorrect.");
        }
    }
    // Id Fournisseur
    fFournisseur = fopen("Fournisseurs.dat","r+b");
    fread(&fourni,sizeof(struct Fournisseur),1,fFournisseur);
    while (fourni == 0)
    {
        printf("Entrez l'ID du fournisseur : ");
        scanf("%ld",&(ptemp->IdFournisseur));
        if(ptemp->IdFournisseur != 0 && VerifFournisseur(IdFourni,ptemp->IdFournisseur,nFourni) == 1)
        {
            fourni = 1;
        }
        else
        {
            printf("  >> ERREUR : Le fournisseur n'existe pas.\n");
        }
    }
    if (fFournisseur!= NULL)
    {
        fclose(fFournisseur);
    }
    // Date Achat
    while (DateCorrect == 0)
    {
        fflush(stdin);
        printf("Entrez la date d'achat valide sous le format (JJ/MM/AAAA) : ");
        gets(date);
        sscanf(date,"%hd/%hd/%hd",&j,&m,&a);
        DateCorrect = VerifDate(j,m,a);
        if (DateCorrect == 1)
        {
            ptemp->DateAchat.Jour = j;
            ptemp->DateAchat.Mois = m;
            ptemp->DateAchat.Annee = a;
        }
    }
    //Date Peremption
    DateCorrect = 0;
    while(DateCorrect == 0)
    {
        fflush(stdin);
        printf("Entrez la date de peremption sous le format (JJ/MM/AAAA) : ");
        gets(date);
        if(&date[0] == '\0')
        {
            DateCorrect = 1;
        }
        else
        {
            sscanf(date,"%hd/%hd/%hd",&jp,&mp,&ap);
            DateCorrect = VerifDate(jp,mp,ap);
        }
        if (DateCorrect == 1 && (ap>=a && mp>=m && jp>=j))
        {
            ptemp->DatePeremption.Jour = jp;
            ptemp->DatePeremption.Mois = mp;
            ptemp->DatePeremption.Annee = ap;

        }
        else
        {
            printf("Impossible d'acheter un produit perime.\n");
            DateCorrect = 0;
        }
    }
    // Prix Unitaire
    while (prixCorrect == 0)
    {
        printf("Entrez le prix unitaire du produit sous le format x.xx: ");
        gets(prix);
        p = strchr(prix,carac);
        if(p==NULL || (*(p+1) == '\0' || *(p+2) == '\0' ||*(p+3) == '\0'))
        {
            prixCorrect = 1;
            sscanf(prix,"%f",&(ptemp->PrixUnitaire));
        }
        else
        {
            printf("Prix invalide\n");
        }
    }
    // Quanttité
    do
    {
        printf("Entrez le quantitee du produit achete : ");
        scanf("%d",&quant);
    }
    while (quant<1);
    ptemp->Quantite = quant;

    return 1;
}

/*****************************************************************************************************************************************************************************/

int VerifDate(short j, short m, short a)
{
    int ret=0;
    if ((a<1583) || ((m>12) || (m<1)) || ((j>31) || (j<1)))
    {
        printf("La date est invalide.\n");
    }
    else
    {
        if (m==4 || m==6 || m==9 || m==11)
        {
            if (j==31)
            {
                printf("La date est invalide.\n");
            }
            else
            {
                ret = 1;
            }
        }
        else
        {
            if (m==2)
            {
                if ((a%400==0) || (a%4==0 && a%100!=0))
                {
                    if (j>29)
                    {
                        printf("La date est invalide.\n");
                    }
                    else
                    {
                        ret = 1;
                    }
                }
                else
                {
                    if (j>28)
                    {
                        printf("La date est invalide.\n");
                    }
                    else
                    {
                        ret = 1;
                    }
                }
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}

/*****************************************************************************************************************************************************************************/

void AfficheAchat(int i)
{
    struct Acheter tempfile;
    FILE * fAchat = NULL;
    fAchat = fopen("Achats.dat","rb");
    if(fAchat == NULL)
    {
        printf("  >> ERREUR : Impossible d'ouvrir le fichier Achats.dat.\n");
    }
    else
    {
        fseek(fAchat,i*sizeof(struct Acheter),SEEK_SET);
        fread(&tempfile,sizeof(struct Acheter),1,fAchat);
        fclose(fAchat);
        printf("Id du produit : ");
        puts(tempfile.IdProduit);
        printf("Id du fournisseur : %ld\n",tempfile.IdFournisseur);
        printf("Date d'achat : %d/%d/%d\n",tempfile.DateAchat.Jour,tempfile.DateAchat.Mois,tempfile.DateAchat.Annee);
        printf("Date de peremption : %d/%d/%d\n",tempfile.DatePeremption.Jour,tempfile.DatePeremption.Mois,tempfile.DatePeremption.Annee);
        printf("Pix unitaire : %.2f\n",tempfile.PrixUnitaire);
        printf("Quantitee : %d\n",tempfile.Quantite);
        printf("\n");
    }
}

/*****************************************************************************************************************************************************************************/

int Validite(struct Acheter temp, int Nachat)
{
    struct Acheter tempfile;
    int valide = 1;
    int i=0, DiffDate=0, DiffDate2=0;
    FILE * fAchat = NULL;
    fAchat = fopen("Achats.dat","rb");
    if(fAchat == NULL)
    {
        printf("  >> ERREUR : Impossible d'ouvrir le fichier Achats.dat.\n");
    }
    else
    {
        while(i<Nachat && valide == 1)
        {
            fread(&tempfile,sizeof(struct Acheter),1,fAchat);
            if((strcmp(temp.IdProduit, tempfile.IdProduit)==0) && (temp.IdFournisseur == tempfile.IdFournisseur) && (ComparaisonDate(temp.DateAchat, tempfile.DateAchat)==0) && (ComparaisonDate(temp.DatePeremption, tempfile.DatePeremption)==0))
            {
                valide = 0;
            }
            else
            {
                if((strcmp(temp.IdProduit, tempfile.IdProduit)==0) && (temp.IdFournisseur == tempfile.IdFournisseur) && (ComparaisonDate(temp.DateAchat, tempfile.DateAchat)==0) && (temp.PrixUnitaire != tempfile.PrixUnitaire))
                {
                    DiffDate = SoustractionDate(temp.DatePeremption,tempfile.DateAchat);
                    DiffDate2 = SoustractionDate(tempfile.DatePeremption,tempfile.DateAchat);
                    if(DiffDate>1 && DiffDate2>1)
                    {
                        valide = 0;
                    }
                }
                i++;
            }
        }
        fclose(fAchat);
    }
	return valide;
}

/*****************************************************************************************************************************************************************************/

//Retourne 1 si la date est avant 0 si après
int ComparaisonDate(struct Date Date1, struct Date Date2)
{
    int ret;
    if(Date1.Annee != Date2.Annee)
    {
        if(Date1.Annee < Date2.Annee)
        {
            ret = 1;
        }
    }
    else
    {
        if(Date1.Mois != Date2.Mois)
        {
            if(Date1.Mois < Date2.Mois)
            {
                ret = 1;
            }
        }
        else
        {
            if(Date1.Jour != Date2.Jour)
            {
                if(Date1.Jour < Date2.Jour)
                {
                    ret = 1;
                }
            }
            else
                ret = 0;
        }
    }
    return ret;
}

/*****************************************************************************************************************************************************************************/

int SoustractionDate(struct Date Date1, struct Date Date2)
{
    int j1, m1, a1, j2, m2, a2;
    time_t sec1,sec2, diff;
    struct tm date1,date2;

    j1=Date1.Jour;
    m1=Date1.Mois;
    a1=Date1.Annee;
    j2=Date2.Jour;
    m2=Date2.Mois;
    a2=Date2.Annee;

    date1.tm_mday=j1-1;
    date1.tm_mon=m1-1;
    date1.tm_year=a1-1900;
    date1.tm_hour=0;
    date1.tm_min=0;
    date1.tm_sec=0;
    sec1=mktime(&date1);
    date2.tm_mday=j2-1;
    date2.tm_mon=m2-1;
    date2.tm_year=a2-1900;
    date2.tm_hour=0;
    date2.tm_min=0;
    date2.tm_sec=0;
    sec2=mktime(&date2);
    diff = difftime(sec2, sec1);
    diff = (diff/86400);
    return diff;
}

/*****************************************************************************************************************************************************************************/

void InsertionIndDate (struct Acheter temp, struct IndDate IndexDate[], int nachat)
{
    int i = nachat-1;
    while(i>=0 && ComparaisonDate(temp.DatePeremption, IndexDate[i].DatePeremption)<0)
    {
        IndexDate[i+1] = IndexDate[i];
        i--;
    }
    IndexDate[i+1].DatePeremption = temp.DatePeremption;
    strcpy(IndexDate[i+1].IdProduit, temp.IdProduit);
    IndexDate[i+1].Quantite = temp.Quantite;
    IndexDate[i+1].posi = nachat;
}

/*****************************************************************************************************************************************************************************/

void AfficheIndex(struct IndDate index[], int nachat) //
{
    int affSvt = 1,i=0;
    printf("Voici la liste des achats trie par ordre croissant de date de peremption.\nAppuyer sur Entrer pour afficher l'element suivant. \nUne autre touche pour quitter.\n\n");
    while (affSvt && i<nachat)
    {
        AfficheAchat((int)index[i].posi);
        if (getch()!=13)
        {
            affSvt = 0;
        }
        i++;
    }
}

/*****************************************************************************************************************************************************************************/

int RechercheIndQuant(struct IndQuant indexQuant[],struct Acheter temp,int *p)
{
    int ret=-1,i=0;
    while(i<(*p))
    {
        if(strcmp(temp.IdProduit,indexQuant[i].IdProduit)==0)
        {
            ret=i;
            i=*p;
        }
        i++;
    }
    return ret;
}

/*****************************************************************************************************************************************************************************/

int SuppressionIndQuant(struct IndQuant indexQuant[],int *nbrIndexQuant,struct Acheter temp, int Pos)
{
    int retQuant,i;
    retQuant = indexQuant[Pos].Quantite;
    for (i=Pos;i<*nbrIndexQuant-1;i++)
    {
        indexQuant[i]=indexQuant[i+1];
    }
    *nbrIndexQuant -= 1;
    return retQuant;
}

/*****************************************************************************************************************************************************************************/

void InsertionIndQuant(struct IndQuant indexQuant[],int retQuant,struct Acheter temp,int *nbrIndexQuant)
{
    int i=*nbrIndexQuant;
    while (temp.Quantite < indexQuant[i].Quantite)
    {
        indexQuant[i] = indexQuant[i+1];
        i--;
    }
    strcpy(indexQuant[i].IdProduit,temp.IdProduit);
    indexQuant[i].Quantite = temp.Quantite;
    indexQuant[i].Quantite += retQuant;
    (*nbrIndexQuant)++;
}

/*****************************************************************************************************************************************************************************/

void AfficheQuant(struct IndQuant indexQuant[], int i)
{
    printf("\nQuantite : %d\n",indexQuant[i].Quantite);
    puts(indexQuant[i].IdProduit);
}

/*****************************************************************************************************************************************************************************/

void InsertionIndexFournisseur(struct Fournisseur fourni, long IdFourni[], int *pnFourni)
{
    int i=*pnFourni;
    while (i>0 && fourni.IdFournisseur < IdFourni[i-1])
    {
        IdFourni[i] = IdFourni[i-1];
        i--;
    }
    IdFourni[i] = fourni.IdFournisseur;
    (*pnFourni)++;
}

/*****************************************************************************************************************************************************************************/

void CreationFichierProduit()
{
    FILE *fProduit = NULL;
    struct Produit temp;
    int i;
    long PTL = 0, PTO = -1;
    fProduit = fopen("Produits.dat","wb");
    if(fProduit != NULL)
    {
        fwrite(&PTL,sizeof(long),1,fProduit);
        fwrite(&PTO,sizeof(long),1,fProduit);
        for (i=0;i<999;i++)
        {
            temp.posi = (long)i+1;
            fwrite(&temp,sizeof(struct Produit),1,fProduit);
        }
        temp.posi = -1;
        fwrite(&temp,sizeof(struct Produit),1,fProduit);
        fclose(fProduit);
        printf("  >> INFO : Le fichier Produits.dat a ete cree avec succes !\n");
    }
    else
    {
        printf("  >> ERREUR : Impossible de creer le fichier Produits.dat");
    }
}

/*****************************************************************************************************************************************************************************/

char MenuProduit(int nProduit)
{
    char choix[20];
    printf("  === MENU PRODUIT ===");
    printf("\n 1) Ajout d'un nouveau produit.");
    printf("\n 2) Affichage des produits par ordre alphabetique.");
    printf("\n 3) Quitter\n\n");
    printf("  === Il y a %d produit(s) encode(s). ===\n\n",nProduit);
    printf("\n  >> Votre choix : ");
    fflush(stdin);
    gets(choix);
    if(strlen(choix)>1)
    {
        return '9';
    }
    return choix[0];
}

/*****************************************************************************************************************************************************************************/

struct Produit EncodeProduit(int nProduit)
{
    struct Produit temp;
    int retVerifID=0,marcorrect=0,categcorrect=0,nomcorrect=0,volcorrect=0;
    while (retVerifID == -1 || retVerifID == 0)
    {
        printf("Entrez l'ID du produit : ");
        fflush(stdin);
        gets(temp.IdProduit);
        retVerifID = VerifID(temp.IdProduit,nProduit);
        if(retVerifID == -1)
        {
            printf("  >> ERREUR : L'ID saisi est deja enregistre.\n");
        }
        else
        {
            if(retVerifID == 0)
            {
                printf("  >> ERREUR : L'ID saisi est incorrect.\n");
            }
            else
            {
                if(retVerifID == -2)
                {
                    strcpy(temp.IdProduit,"\0");
                    return temp;
                }
            }
        }
    }
    // Encodage Marque du produit
    do
    {
        printf("Entrez la Marque du produit : ");
        fflush(stdin);
        gets(temp.Marque);
        marcorrect = isCharStr(temp.Marque);
    }
    while (marcorrect == 0);
    // Encodage Nom du produit
    do
    {
        printf("Entrez le Nom du produit : ");
        fflush(stdin);
        gets(temp.NomProduit);
        nomcorrect = isCharStr(temp.NomProduit);
    }
    while (nomcorrect == 0);
    // Encodage volume du produit
    do
    {
        printf("Entrez le volume du produit : ");
        fflush(stdin);
        gets(temp.Volume);
        // test validité volume
        volcorrect = VerifVolume(temp.Volume);
    }
    while (volcorrect == 0);
    // Encodage Categorie du produit
    do
    {
        printf("Entrez la Categorie du produit : ");
        fflush(stdin);
        gets(temp.categorie);
        categcorrect = isCharStr(temp.categorie);
    }
    while (categcorrect == 0);
    temp = StrToLower(temp);
    return temp;
}

/*****************************************************************************************************************************************************************************/

void AfficheUnProduit(struct Produit temp)
{
    printf("Id du Produit : ");
    puts(temp.IdProduit);
    printf("Categorie : ");
    puts(temp.categorie);
    printf("Marque : ");
    puts(temp.Marque);
    printf("Nom : ");
    puts(temp.NomProduit);
    printf("Volume : ");
    puts(temp.Volume);
    printf("\n");
}

/*****************************************************************************************************************************************************************************/

long getptl()
{
    long ret;
    FILE *f = NULL;
    f = fopen("Produits.dat","rb");
    if(f == NULL)
    {
        printf("  >> ERREUR : Probleme d'ouverture.\n");
    }
    else
    {
        fread(&ret,sizeof(long),1,f);
        fclose(f);
    }
    return ret;
}

/*****************************************************************************************************************************************************************************/

long getpto()
{
    long ret;
    FILE *f = NULL;
    f = fopen("Produits.dat","rb");
    if(f == NULL)
    {
        printf("  >> ERREUR : Probleme d'ouverture.\n");
    }
    else
    {
        fseek(f,sizeof(long),SEEK_SET);
        fread(&ret,sizeof(long),1,f);
        fclose(f);
    }
    return ret;
}

/*****************************************************************************************************************************************************************************/

void writeptl(long PTL)
{
    FILE *f = NULL;
    f = fopen("Produits.dat","r+b");
    if(f == NULL)
    {
        printf("  >> ERREUR : Probleme d'ouverture.\n");
    }
    else
    {
        fwrite(&PTL,sizeof(long),1,f);
        fclose(f);
    }
}

/*****************************************************************************************************************************************************************************/

void writepto(long PTO)
{
    FILE *f = NULL;
    f = fopen("Produits.dat","r+b");
    if(f == NULL)
    {
        printf("  >> ERREUR : Probleme d'ouverture.\n");
    }
    else
    {
        fseek(f,sizeof(long),SEEK_SET);
        fwrite(&PTO,sizeof(long),1,f);
        fclose(f);
    }
}

/*****************************************************************************************************************************************************************************/

void EcritureFichier(struct Produit NouvElem,int nProduit)
{
    FILE * fProduit = NULL;
    int NbrLu=0;
    struct Produit tempfile;
    long PTL,PTO,posiLu,posiPrec;
    PTL = getptl();
    PTO = getpto();
    fProduit = fopen("Produits.dat","r+b");
    fseek(fProduit,2*sizeof(long)+PTO*sizeof(struct Produit),SEEK_SET);
    posiLu = PTO;
    if (PTL == -1)
    {
        printf("  >> ERREUR : Le fichier de produit est complet.");
    }
    else
    {
        do
        {
            fread(&tempfile,sizeof(struct Produit),1,fProduit);
            posiPrec = posiLu;
            posiLu = tempfile.posi;
            NbrLu++;
        }
        while (NbrLu <= nProduit && StrCmpCaMaNo(tempfile,NouvElem) == -1);
        if(PTO == 0 || NbrLu == 1)
        {
            // Premier par ordre alphabétique ou Le fichier est "vide"
            NouvElem.posi = PTO;
            PTO = PTL;
            fseek(fProduit,2*sizeof(long)+PTL*sizeof(struct Produit),SEEK_SET);
            fread(&tempfile,sizeof(struct Produit),1,fProduit);
            posiLu = tempfile.posi;
            fseek(fProduit,2*sizeof(long)+PTL*sizeof(struct Produit),SEEK_SET);
            fwrite(&NouvElem,sizeof(struct Produit),1,fProduit);
            PTL = posiLu;
            fclose(fProduit);
            writepto(PTO);
            writeptl(PTL);
        }
        else
        {
            if((long)NbrLu >= PTL)
            {
                // Dernier par ordre alphabétique
                NouvElem.posi = posiLu;
                tempfile.posi = PTL;
                fseek(fProduit,2*sizeof(long)+posiLu*sizeof(struct Produit),SEEK_SET);
                fwrite(&tempfile,sizeof(struct Produit),1,fProduit);
                fseek(fProduit,2*sizeof(long)+PTL*sizeof(struct Produit),SEEK_SET);
                fread(&tempfile,sizeof(struct Produit),1,fProduit);
                posiLu = tempfile.posi;
                fseek(fProduit,2*sizeof(long)+PTL*sizeof(struct Produit),SEEK_SET);
                fwrite(&NouvElem,sizeof(struct Produit),1,fProduit);
                PTL = posiLu;
                fclose(fProduit);
                writeptl(PTL);
            }
            else
            {
                // ni premier ni dernier
                NouvElem.posi = posiLu;
                tempfile.posi = PTL;
                fseek(fProduit,2*sizeof(long)+posiPrec*sizeof(struct Produit),SEEK_SET);
                fwrite(&tempfile,sizeof(struct Produit),1,fProduit);
                fseek(fProduit,2*sizeof(long)+PTL*sizeof(struct Produit),SEEK_SET);
                fread(&tempfile,sizeof(struct Produit),1,fProduit);
                posiLu = tempfile.posi;
                fseek(fProduit,2*sizeof(long)+PTL*sizeof(struct Produit),SEEK_SET);
                fwrite(&NouvElem,sizeof(struct Produit),1,fProduit);
                PTL = posiLu;
                fclose(fProduit);
                writeptl(PTL);
            }
        }
    }
}

/*****************************************************************************************************************************************************************************/

int isNbr (char v[])
{
    int ret=1,a,i=0,n=strlen(v);
    while (i<n && ret == 1)
    {
        a = isdigit(v[i]);
        i++;
        if(a==0)
        {
            ret = 0;
        }
    }
    return ret;
}

/*****************************************************************************************************************************************************************************/

int isCharStr (char v[])
{
    int ret=1,i=0,n=strlen(v);
    while (i<n && ret == 1)
    {
        if(isdigit(v[i]) == 1)
        {
            ret = 0;
        }
        i++;
    }
    return ret;
}

/*****************************************************************************************************************************************************************************/

int StrCmpCaMaNo (struct Produit str1, struct Produit str2)
{
    int ret,cmp;
    cmp = strcmp(str1.categorie,str2.categorie);
    if(cmp == -1)
    {
        ret = -1;
    }
    else
    {
        if(cmp == 1)
        {
            ret = 1;
        }
        else
        {
            cmp = strcmp(str1.Marque,str2.Marque);
            if(cmp == -1)
            {
                ret = -1;
            }
            else
            {
                if(cmp == 1)
                {
                    ret = 1;
                }
                else
                {
                    cmp = strcmp(str1.NomProduit,str2.NomProduit);
                    if(cmp == -1)
                    {
                        ret = -1;
                    }
                    else
                    {
                        if(cmp == 1)
                        {
                            ret = 1;
                        }
                        else
                        {
                            ret = -1;
                        }
                    }
                }
            }
        }
    }
    return ret;
}

/*****************************************************************************************************************************************************************************/

struct Produit StrToLower(struct Produit temp)
{
    int i,longu;
    longu = strlen(temp.categorie);
    for(i=0;i<longu;i++)
    {
        if (temp.categorie[i] >= 'A' && temp.categorie[i] <= 'Z')
        {
            temp.categorie[i] += 32;
        }
    }

    longu = strlen(temp.Marque);
    for(i=0;i<longu;i++)
    {
        if (temp.Marque[i] >= 'A' && temp.Marque[i] <= 'Z')
        {
            temp.Marque[i] += 32;
        }
    }

    longu = strlen(temp.NomProduit);
    for(i=0;i<longu;i++)
    {
        if (temp.NomProduit[i] >= 'A' && temp.NomProduit[i] <= 'Z')
        {
            temp.NomProduit[i] += 32;
        }
    }
    return temp;
}

/*****************************************************************************************************************************************************************************/

int VerifID (char NouvID[], int nProduit)
{
    int IdDejPres,i=0,ret=0;
    struct Produit temp;
    FILE *fProduit = NULL;
    fProduit = fopen("Produits.dat","rb");
    if(fProduit == NULL)
    {
        printf("  >> ERREUR : Impossible d'acceder au fichier Produit.");
    }
    else
    {
        while(i<nProduit)
        {
            fseek(fProduit,2*sizeof(long)+i*sizeof(struct Produit),SEEK_SET);
            fread(&temp,sizeof(struct Produit),1,fProduit);
            if(strcmp(temp.IdProduit,NouvID) == 0)
            {
                // ID déja présent
                IdDejPres = 1;
                i = nProduit;
            }
            i++;
        }
    }
    if(IdDejPres == 1)
    {
        return -1;
    }

    if(strlen(NouvID) == 13 && isNbr(NouvID) == 1)
    {
        ret = 1;
    }
    if(strlen(NouvID) == 0)
    {
        ret = -2;
    }
    return ret;
}

/*****************************************************************************************************************************************************************************/

int VerifVolume(char volume[])
{
    char code[9],tabCode[20][9]={"CEL","CELL","CCEL","CCELL","CCCEL","CCCELL","CCCCEL","CCCCELL","CVCEL","CVCELL","CVCCEL","CVCCELL","CVCCCEL","CVCCCELL","CCVCEL","CCVCELL","CCVCCEL","CCVCCELL","CCCVCEL","CCCVCELL"};
    int ret,i;
    for(i=0;i<strlen(volume);i++)
    {
        if(volume[i] == ',' || volume[i] == '.')
        {
            code[i] = 'V';
        }
        if(volume[i] == ' ')
        {
            code[i] = 'E';
        }
        if(isdigit(volume[i]) == 1)
        {
            code[i] = 'C';
        }
        if((volume[i] >= 'A' && volume[i] <= 'Z') || (volume[i] >= 'a' && volume[i] <= 'z'))
        {
            code[i] = 'L';
        }
    }
    code[strlen(volume)] = '\0';
    i=0;
    while(i<20)
    {
        if(strcmp(code,tabCode[i]) == 0)
        {
            ret = 1;
            i=20;
        }
        i++;
    }
    return ret;
}

/*****************************************************************************************************************************************************************************/

int VerifFournisseur (long IdFourni[], long NouvID,int nFourni)
{
    int i=0,ret=0;
    while(i<nFourni && NouvID>IdFourni[i])
    {
        i++;
    }
    if(NouvID == IdFourni[i])
    {
        ret = 1;
    }
    return ret;
}
