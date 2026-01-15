using UnityEngine;

public class Enemy : MonoBehaviour{
    [SerializeField] EnemyData data;

    Transform[] waypointList;

    int currentHealth;
    public int currentIndex = 0;

    void Start(){
        currentHealth = data.health;

        if (GameManager.Instance.currentLevel == 1){
            Transform wp = GameObject.Find("Level1Waypoints").transform;
            SetWaypoints(wp);
        }
        else if (GameManager.Instance.currentLevel == 2){
            Transform wp = GameObject.Find("Level2Waypoints").transform;
            SetWaypoints(wp);
        }
    }

    void SetWaypoints(Transform waypointParent){
        waypointList = new Transform[waypointParent.childCount];

        for (int i = 0; i < waypointList.Length; i++){
            waypointList[i] = waypointParent.GetChild(i);
        }
    }


    void Update(){
        Transform targetWaypoint = waypointList[currentIndex];
        
        //Move to the waypoint
        this.transform.position = Vector3.MoveTowards(transform.position, targetWaypoint.position, data.speed  * Time.deltaTime);

        Vector2 direction = targetWaypoint.position - transform.position;
        float angle = Mathf.Atan2(direction.y, direction.x) * Mathf.Rad2Deg + 90f;
        transform.rotation = Quaternion.Euler(0f, 0f, angle);

        if (Vector3.Distance(transform.position, targetWaypoint.position) < 0.05f){
            if (currentIndex == waypointList.Length - 1){
                Destroy(this.gameObject);
                GameData.Health -= data.damage;

                if (GameData.Health <= 0){
                    GameData.Health = 0;
                    GameManager.Instance.Lose();
                }
            }
            else{
                currentIndex++;
            }
        }
    }


    public void TakeDamage(int damage){
        currentHealth -= damage;

        if (currentHealth <= 0){
            GameData.Money += data.moneyEarned;
            Destroy(gameObject);
        }
    }
}
