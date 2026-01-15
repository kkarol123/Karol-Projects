using UnityEngine;

[CreateAssetMenu(fileName = "EnemyData", menuName = "Enemy Data")]
public class EnemyData : ScriptableObject{
    public GameObject prefab;

    public float speed;
    public int moneyEarned;
    public int damage;
    public int health;  
}