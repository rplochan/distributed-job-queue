from fastapi import FastAPI, HTTPException
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

SUPPORTED_JOBS = {
    "add",
    "subtract",
    "mult",
    "multiply",
    "div",
    "power",
    "factorial",
    "fibonacci",
    "prime_check",
    "sleep",
    "hash"
}


@app.get("/")
def root():
    return {"message": "Job queue api is running"}


# Validate job type
if job.type not in SUPPORTED_JOBS:
    raise HTTPException(
        status_code=400,
        detail="Unsupported job type"
    )

# Validate hash input
if job.type == "hash" and not job.input:
    raise HTTPException(
        status_code=400,
        detail="Hash input cannot be empty"
    )

# Validate sleep duration
if job.type == "sleep" and job.duration < 0:
    raise HTTPException(
        status_code=400,
        detail="Sleep duration cannot be negative"
    )

# Validate division
if job.type == "div" and job.b == 0:
    raise HTTPException(
        status_code=400,
        detail="Division by zero"
    )

# Validate factorial
if job.type == "factorial" and job.a < 0:
    raise HTTPException(
        status_code=400,
        detail="Factorial requires a non-negative integer"
    )

# Validate fibonacci
if job.type == "fibonacci" and job.a < 0:
    raise HTTPException(
        status_code=400,
        detail="Fibonacci requires a non-negative integer"
    )

# Validate power
if job.type == "power" and job.b < 0:
    raise HTTPException(
        status_code=400,
        detail="Negative exponent is unsupported"
    )

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
        f"{job_id}|{job.type}|{job.a}|{job.b}|"
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

    cursor = db.cursor()

    try:
        cursor.execute(
            """
            SELECT status,result_int, result_text, error
            FROM jobs
            WHERE id = %s
            """,
            (job_id,)
        )

        result = cursor.fetchone()

        if result is None:
            return {"error": "Job not found"}

        status, result_int, result_text, error = result
        
        return {
            "job_id": job_id,
            "status": status,
            "result_int": result_int,
            "result_text": result_text,
            "error": error
        }

        finally:
            cursor.close()
        

