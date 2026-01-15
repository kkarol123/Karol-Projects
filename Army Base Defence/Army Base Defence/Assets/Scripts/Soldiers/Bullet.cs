using UnityEngine;

public class Bullet : MonoBehaviour{
    float speed = 10f;
    Rigidbody2D rb;

    void Awake(){
        rb = GetComponent<Rigidbody2D>();
    }

    void Start(){
        rb.linearVelocity = -transform.up * speed;
        Destroy(gameObject, 3f);
    }

    void OnTriggerEnter2D(Collider2D  other){
        if (other.gameObject.CompareTag("Enemy")){
            Enemy enemy = other.gameObject.GetComponent<Enemy>();
            enemy.TakeDamage(1); 

            Destroy(gameObject);
        }
    }
}
