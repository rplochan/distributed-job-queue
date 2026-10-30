from fastapi import FastAPI
from pydantic import BaseModel
import psycopg2
import redis
import json

app = FastAPI()

#connect postgresql database
db = psycopg2.connect(
    host="localhost",
    port=5432,
    database="job_queue",
    user="postgres",
    password="chatapp123"
)


redis_client = redis.Redis(
    host = "localhost",
    port = 6379,
    decode_responses=True
)


class JobRequest(BaseModel):
    type: str
    a: int = 0
    b: int = 0
    duration: int = 0
    input : str = ""

@app.get("/")
def root():
    return {"message": "Job queue api is running"}

@app.post("/jobs")
def create_job(job : JobRequest):
    # generate a unique id
    job_id = redis_client.incr("job_id")

    input_data = {
        "a": job.a,
        "b": job.b,
        "duration": job.duration,
        "input": job.input
    }


    #save job in postgresql
    cursor = db.cursor()

    try:
        cursor.execute(
            """
            INSERT INTO jobs (id, type, input_data, status)
            VALUES (%s, %s, %s, %s)
            """,
            (job_id, job.type, json.dumps(input_data), "pending")
        )

        db.commit()

    except Exception:
        db.rollback()
        raise
    
    finally:
        cursor.close()

    #serialize the job for cpp parser.
    job_data = (
        f"{job_id}|{job.type}|{job.a}|{job.b}"
        f"{job.duration}|{job.input}"
    )

    #store the initial status
    redis_client.set(
        f"job:{job_id}:status",
        "pending"
    )

    #enqueue the job.
    redis_client.lpush("jobs", job_data)

    return {
        "job_id": job_id,
        "status": "pending"
    }




@app.get("/jobs/{job_id}")
def get_job_status(job_id : int):

    status = redis_client.get(
        f"job:{job_id}:status"
    )

    if status is None:
        return {
            "error": "Job not found"
        }

    return {
        "job_id": job_id,
        "status": status
    }


