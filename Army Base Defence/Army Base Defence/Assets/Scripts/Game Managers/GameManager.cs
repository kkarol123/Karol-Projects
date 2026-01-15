using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.SceneManagement;
using System.IO;
using TMPro;
using System.Runtime.Serialization.Formatters.Binary;


public class GameManager : MonoBehaviour{
    public static GameManager Instance;

    Transform level1SpawnWaypoint;
    Transform level2SpawnWaypoint;

    float normalDelay = 2f;
    float bigPauseDelay = 3.5f;

    Button level1StartButton;
    Button level2StartButton;
    Button startGameButton;
    Button quitGameButton;
    Button mainMenuButton;
    Button addToHighScoreButton;
    Button viewHighScoresButton;
    Button clearHighScoresButton;

    TMP_Text moneyText;
    TMP_Text healthText;

    public int currentLevel = 0;

    [SerializeField] LevelData level1Data;
    [SerializeField] LevelData level2Data;
    bool doneSpawning = false;


    void Awake(){
        //Singleton guard
        if (Instance != null && Instance != this){
            Destroy(gameObject);
            return;
        }

        Instance = this;
        DontDestroyOnLoad(gameObject);

        //this is here because everytime a scene loads, UI button and Text references become invalid and need to be rebinded
        SceneManager.sceneLoaded += OnSceneLoaded;
    }





    void OnSceneLoaded(Scene scene, LoadSceneMode mode){
        //Spawnpoints
        level1SpawnWaypoint = GameObject.Find("Waypoint1")?.transform;
        level2SpawnWaypoint = GameObject.Find("Waypoint1")?.transform;

        //Buttons
        level1StartButton = GameObject.Find("Level1StartButton")?.GetComponent<Button>();
        level2StartButton = GameObject.Find("Level2StartButton")?.GetComponent<Button>();
        startGameButton = GameObject.Find("StartGameButton")?.GetComponent<Button>();
        quitGameButton = GameObject.Find("QuitGameButton")?.GetComponent<Button>();
        mainMenuButton = GameObject.Find("MainMenuButton")?.GetComponent<Button>();
        addToHighScoreButton = GameObject.Find("AddToHighScoreButton")?.GetComponent<Button>();
        viewHighScoresButton = GameObject.Find("ViewHighScoresButton")?.GetComponent<Button>();
        clearHighScoresButton = GameObject.Find("ClearHighScoresButton")?.GetComponent<Button>();

        //Text
        moneyText  = GameObject.Find("MoneyText")?.GetComponent<TMP_Text>();
        healthText = GameObject.Find("HealthText")?.GetComponent<TMP_Text>();


        if (level1StartButton != null){
            level1StartButton.onClick.AddListener(OnLevel1StartButtonClicked);
        }
        if (level2StartButton != null){
            level2StartButton.onClick.AddListener(OnLevel2StartButtonClicked);
        }
        if (startGameButton != null){
            startGameButton.onClick.AddListener(OnStartGameButtonClicked);
        }
        if (addToHighScoreButton != null){
            addToHighScoreButton.onClick.AddListener(OnAddToHighScoresButtonClicked);
        }
        if (viewHighScoresButton != null){
            viewHighScoresButton.onClick.AddListener(OnViewHighScoresButtonClicked);
        }
        if (clearHighScoresButton != null){
            clearHighScoresButton.onClick.AddListener(OnClearHighScoresClicked);
        }

        // Main Menu button which works in all scenes where it is used
        if (mainMenuButton != null){
            mainMenuButton.onClick.RemoveAllListeners();
            mainMenuButton.onClick.AddListener(OnMainMenuButtonClicked);
        }

        // Quit button which works in all scenes where it is used
        if (quitGameButton != null){
            quitGameButton.onClick.RemoveAllListeners();
            quitGameButton.onClick.AddListener(OnQuitGameButtonClicked);
        }
    }

    void Update(){
        if (moneyText != null){
            moneyText.text = GameData.Money.ToString();
        }

        if (healthText != null){
            healthText.text = "HP: " + GameData.Health.ToString();
        }

        if (doneSpawning == true){
            GameObject[] enemiesLeft = GameObject.FindGameObjectsWithTag("Enemy");

            if (enemiesLeft.Length == 0){
                doneSpawning = false;
                LoadNextLevelWithDelay();
            }
        }
    }





    void OnStartGameButtonClicked(){
        //Reset Stats
        GameData.Money = 100;
        GameData.Health = 100;
        
        SceneManager.LoadScene("Level1");
    }
    void OnQuitGameButtonClicked(){
        //this is so on the unity game tab, if i press quit, it will quit from there also.
        #if UNITY_EDITOR
            UnityEditor.EditorApplication.isPlaying = false;
        #else
            Application.Quit();
        #endif
    }
    void OnMainMenuButtonClicked(){
        SceneManager.LoadScene("WelcomeScene");
    }
    void GoToWinScreen(){
        SceneManager.LoadScene("WinScene");
    }
    void GoToLoseScreen(){
        SceneManager.LoadScene("LoseScene");
    }
    void OnLevel1StartButtonClicked(){
        currentLevel = 1;

        Destroy(level1StartButton.gameObject);
        StartCoroutine(SpawnEnemiesLoop(level1Data, level1SpawnWaypoint));
    }
    void OnLevel2StartButtonClicked(){
        currentLevel = 2;

        Destroy(level2StartButton.gameObject);
        StartCoroutine(SpawnEnemiesLoop(level2Data, level2SpawnWaypoint));
    }




    void OnAddToHighScoresButtonClicked(){
        List<PlayerData> highScores = new List<PlayerData>();

        if (File.Exists(Application.persistentDataPath + "/highScores.dat") == true){
            BinaryFormatter bf = new BinaryFormatter();
            FileStream file = File.Open(Application.persistentDataPath + "/highScores.dat", FileMode.Open);
            highScores = (List<PlayerData>)bf.Deserialize(file);
            file.Close();
        }

        PlayerData data = new PlayerData();
        data.money = GameData.Money;
        data.health = GameData.Health;

        highScores.Add(data);

        BinaryFormatter bfSave = new BinaryFormatter();
        FileStream saveFile = File.Create(Application.persistentDataPath + "/highScores.dat");
        bfSave.Serialize(saveFile, highScores);
        saveFile.Close();

        Destroy(addToHighScoreButton.gameObject);
    }

    void OnClearHighScoresClicked(){
        if (File.Exists(Application.persistentDataPath + "/highScores.dat")){
            File.Delete(Application.persistentDataPath + "/highScores.dat");
        }
    }

    void OnViewHighScoresButtonClicked(){
        SceneManager.LoadScene("HighScoresScene");
    }









    IEnumerator SpawnEnemiesLoop(LevelData level, Transform spawn){
        doneSpawning = false; //reset to false for level 2

        foreach (EnemyData enemy in level.enemies){
            if (enemy == null){
                yield return new WaitForSeconds(bigPauseDelay);
                continue;
            }

            Instantiate(enemy.prefab, spawn.position, Quaternion.identity);
            yield return new WaitForSeconds(normalDelay);
        }

        doneSpawning = true;
    }


    public void LoadNextLevelWithDelay(){
        StartCoroutine(Delay());
    }

    IEnumerator Delay(){
        yield return new WaitForSeconds(3f);

        
        if (currentLevel == 1){
            //This is so if the user has no money between the rounds, he can atleast place some soldiers.
            if (GameData.Money < 100){
                GameData.Money = 100;
            }
            
            SceneManager.LoadScene("Level2");
        }
        else{
            SceneManager.LoadScene("WinScene");
        }
    }

    public void Lose(){
        StopAllCoroutines();  //stop the spawning coroutine

        SceneManager.LoadScene("LoseScene");

        //This is to reset the game to the next time the user plays (without closing and reopening the game)
        GameData.Money = 100;
        GameData.Health = 100;
    }


    //unsubscribe from OnSceneLoaded to reduce memory leaks, etc.
    void OnDestroy(){
        SceneManager.sceneLoaded -= OnSceneLoaded;
    }
}